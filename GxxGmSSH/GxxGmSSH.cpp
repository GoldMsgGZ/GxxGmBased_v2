#include <iostream>

#include "GxxGmSSH.h"
#include <WinSock.h>

#pragma comment(lib, "libssh2.lib")
#pragma comment(lib, "ws2_32.lib")

static int waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{
	struct timeval timeout;
	int rc;
	fd_set fd;
	fd_set *writefd = NULL;
	fd_set *readfd = NULL;
	int dir;

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	FD_ZERO(&fd);

	FD_SET(socket_fd, &fd);

	/* now make sure we wait in the correct direction */
	dir = libssh2_session_block_directions(session);

	if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
		readfd = &fd;

	if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
		writefd = &fd;

	rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);

	return rc;
}

GxxGmSSH::GxxGmSSH()
{

}

GxxGmSSH::~GxxGmSSH()
{

}

int GxxGmSSH::Initialize()
{
	int errCode = 0;

#ifdef WIN32
	WSADATA wsadata;
	errCode = WSAStartup(MAKEWORD(2,0), &wsadata);

	if (errCode != 0)
	{
		printf("WSAStartup failed with error: %d\n", errCode);
		return errCode;
	}
#endif

	errCode = libssh2_init(0);
	if (errCode != 0)
	{
		printf ("libssh2 initialization failed (%d)\n", errCode);
		return errCode;
	}

	return errCode;
}

void GxxGmSSH::Close()
{
	libssh2_exit();

	WSACleanup();
}

int GxxGmSSH::Login(const char *ipaddr, const char *username, const char *password)
{
	int errCode = 0;

	// SOCKET连接到目标服务器
	unsigned long hostaddr = inet_addr(ipaddr);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(22);
	sin.sin_addr.s_addr = hostaddr;

	errCode = connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in));
	if (errCode != 0)
	{
		printf("failed to connect!\n");
		return errCode;
	}

	// 创建SSH Session
	ssh2_session = libssh2_session_init();
	if (ssh2_session == NULL)
		return -2;

	libssh2_session_set_blocking(ssh2_session, 0);

	// 
	while ((errCode = libssh2_session_handshake(ssh2_session, sock)) == LIBSSH2_ERROR_EAGAIN);

	if (errCode)
	{
		printf("Failure establishing SSH session: %d\n", errCode);
		return -3;
	}

	LIBSSH2_KNOWNHOSTS *libssh2_knownhosts = libssh2_knownhost_init(ssh2_session);
	if (libssh2_knownhosts == NULL)
	{
		return -3;
	}

	size_t len = 0;
	int type = 0;
	const char *fingerprint = libssh2_session_hostkey(ssh2_session, &len, &type);

	if(fingerprint) 
	{
        struct libssh2_knownhost *host;

#if LIBSSH2_VERSION_NUM >= 0x010206
        /* introduced in 1.2.6 */
        int check = libssh2_knownhost_checkp(libssh2_knownhosts, ipaddr, 22,
                                             fingerprint, len,
                                             LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                             LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                             &host);
#else
        /* 1.2.5 or older */
        int check = libssh2_knownhost_check(libssh2_knownhosts, ipaddr,
                                            fingerprint, len,
                                            LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                            LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                            &host);
#endif
        fprintf(stderr, "Host check: %d, key: %s\n", check,
                (check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH)?
                host->key:"<none>");

        /*****
         * At this point, we could verify that 'check' tells us the key is
         * fine or bail out.
         *****/
    }
    else
	{
        /* eeek, do cleanup here */
        return 3;
    }

    libssh2_knownhost_free(libssh2_knownhosts);

	if ( strlen(password) != 0 ) {
		/* We could authenticate via password */
		while ((errCode = libssh2_userauth_password(ssh2_session, username, password)) == LIBSSH2_ERROR_EAGAIN);

		if (errCode)
		{
			printf("Authentication by password failed.\n");
			return errCode;
		}
	}
	else
	{
		/* Or by public key */
		while ((errCode = libssh2_userauth_publickey_fromfile(ssh2_session, username,
			"/home/user/"
			".ssh/id_rsa.pub",
			"/home/user/"
			".ssh/id_rsa",
			password)) ==
			LIBSSH2_ERROR_EAGAIN);
		if (errCode) {
			printf("\tAuthentication by public key failed\n");
		}
	}

	return errCode;
}

void GxxGmSSH::Logout()
{
	int errCode = 0;
	int exitcode = 127;
	char *exitsignal = (char *)"none";


	while( (errCode = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN )
		waitsocket(sock, ssh2_session);

	if( errCode == 0 )
	{
		exitcode = libssh2_channel_get_exit_status( channel );
		libssh2_channel_get_exit_signal(channel, &exitsignal, NULL, NULL, NULL, NULL, NULL);
	}

	if (exitsignal)
		printf("\nGot signal: %s\n", exitsignal);
	else 
		printf("\nEXIT: %d\n", exitcode);

	libssh2_channel_free(channel);
	channel = NULL;

	libssh2_session_disconnect(ssh2_session, "Normal Shutdown, Thank you for playing");
	libssh2_session_free(ssh2_session);

#ifdef WIN32
	closesocket(sock);
#else
	close(sock);
#endif

	printf("all done\n");

	
}

int GxxGmSSH::Exec(const char *commandline, std::string &result)
{
	int errCode = 0;
	int bytecount = 0;

	while( (channel = libssh2_channel_open_session(ssh2_session)) == NULL
		&& libssh2_session_last_error(ssh2_session,NULL,NULL,0) == LIBSSH2_ERROR_EAGAIN )
	{
		waitsocket(sock, ssh2_session);
	}

    if( channel == NULL )
    {
        printf("Error\n");
        return -1;
    }

    while( (errCode = libssh2_channel_exec(channel, commandline)) == LIBSSH2_ERROR_EAGAIN )
    {
        waitsocket(sock, ssh2_session);
    }
    if( errCode != 0 )
    {
        printf("Error\n");
        return -2;
    }
    for( ;; )
    {
        /* loop until we block */
        int rc;
        do
        {
			char buffer[0x4000] = {0};
            rc = libssh2_channel_read( channel, buffer, sizeof(buffer) );
            if( rc > 0 )
            {
                int i;
                bytecount += rc;
                //printf("We read:\n");
                for( i=0; i < rc; ++i )
                    fputc( buffer[i], stderr);
                printf("\n");
            }
            else {
                //if( rc != LIBSSH2_ERROR_EAGAIN )
                //    /* no need to output this for the EAGAIN case */
                //    printf("libssh2_channel_read returned %d\n", rc);
            }
        }
        while( rc > 0 );

        /* this is due to blocking that would occur otherwise so we loop on
           this condition */
        if( rc == LIBSSH2_ERROR_EAGAIN )
        {
            waitsocket(sock, ssh2_session);
        }
        else
            break;
    }

	return 0;
}