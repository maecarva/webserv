#include "Webserv.hpp"

void executeCgi( std::vector< std::string > command, char **envp, const std::string& input )
{
	char **argv = new char *[command.size() + 1];

	for ( size_t i = 0; i < command.size(); ++i )
		argv[i] = const_cast<char *>( command[i].c_str() );
	argv[command.size()] = NULL;

	int pipe_in[2];

	if ( pipe( pipe_in ) == -1 )
	{
		perror( "pipe" );
		return ;
	}

	int pipe_out[2];

	if ( pipe( pipe_out ) == -1 )
	{
		close( pipe_in[0] ); close( pipe_in[1] );
		perror( "pipe" );
		return ;
	}

	pid_t pid = fork();

	if ( pid == -1 )
	{
		close( pipe_in[0] ); close( pipe_in[1] );
		close( pipe_out[0] ); close( pipe_out[1] );
		perror( "fork" );
		return ;
	}

	if ( pid == 0 )
	{
		close( pipe_in[1] ); close( pipe_out[0] );
		dup2( pipe_in[0], STDIN_FILENO );
		dup2( pipe_out[1], STDOUT_FILENO );
		close( pipe_in[0] ); close( pipe_out[1] );

		execve( argv[0], argv, envp );
		perror( "excve" );
		delete[] argv;
		exit( 1 ); // Faire gaffe en gerant les leaks
	}
	else
	{
		close( pipe_in[0] ); close( pipe_out[1] );
		write( pipe_in[1], input.c_str(), input.size() );
		close( pipe_in[1] );

		char buffer[1024];
		ssize_t bytesRead;
		while ( ( bytesRead = read( pipe_out[0], buffer, sizeof( buffer ) ) ) > 0)
			write( STDOUT_FILENO, buffer, bytesRead );

		close( pipe_out[0] );

		waitpid( pid, NULL, 0 );
	}

	delete[] argv;
}
