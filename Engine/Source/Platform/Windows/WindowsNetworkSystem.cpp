#if defined(CATALYST_PLATFORM_WINDOWS)
//Header file.
#include <Systems/NetworkSystem.h>

/*
*	Initializes the network subsystem.
*/
void NetworkSystem::SubInitialize() NOEXCEPT
{
#if 0
	//Remember the error code.
	asio::error_code error_code;

	//Create the IO context.
	asio::io_context io_context;

	//Create the endpoint.
	asio::ip::tcp::endpoint endpoint{ asio::ip::make_address("93.184.216.34", error_code), 80 };

	//Create the socket.
	asio::ip::tcp::socket socket{ io_context };

	//Connect the socket.
	socket.connect(endpoint, error_code);

	if (error_code)
	{
		ASSERT(false, error_code.message());
	}

	if (socket.is_open())
	{
		const std::string request
		{
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n"
		};

		socket.write_some(asio::buffer(request.data(), request.size()), error_code);

		if (error_code)
		{
			ASSERT(false, error_code.message());
		}

		socket.wait(socket.wait_read);

		const size_t bytes_available{ socket.available() };

		if (bytes_available > 0)
		{
			std::vector<char> buffer;
			buffer.resize(bytes_available);

			socket.read_some(asio::buffer(buffer.data(), buffer.size()), error_code);

			for (const char character : buffer)
			{
				PRINT_TO_OUTPUT(character);
			}
		}
	}
#endif
}
#endif