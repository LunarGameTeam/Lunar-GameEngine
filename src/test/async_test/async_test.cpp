
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/bind.hpp>
#include <string>
#include <vector>

std::vector<char> data; // as data member
// then pass buffer by
std::fstream fs;
boost::asio::streambuf b;

void send_response2(const boost::system::error_code &err, const size_t &bytes)
{
	if (err == boost::asio::error::eof)
	{
		std::cout << "Done";
		// analyze your buf2 here if it is what you wanted to read
		// here you can start sending data
	}
	else if (err)
	{ /* problem */
	}
	else
	{
		// call again async_read with send_response like
		boost::asio::async_read(fs, b, boost::asio::transfer_all(), boost::bind(&send_response2,
																				boost::asio::placeholders::error,
																				boost::asio::placeholders::bytes_transferred));
	}
}

int main()
{
	boost::asio::windows::stream_handle hand;
	hand.
	fs.open("io.txt", std::ios::out);
	boost::asio::async_read(fs, b, boost::asio::transfer_all(), boost::bind(&send_response2,
																			boost::asio::placeholders::error,
																			boost::asio::placeholders::bytes_transferred));
}