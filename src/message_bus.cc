/*
	This file is part of bro_amqp_plugin.

	Copyright (c) 2015-2016, Packetsled. All rights reserved.

	tcplog is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	tcplog is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with tcplog.  If not, see <http://www.gnu.org/licenses/>.

 */
// Aaron Eppert - PacketSled - 2015-2016

#include "message_bus.h"

//using namespace plugin::PS_amqp;

//
// connStr -> "<password>:<login>@<hostname>:<port>"
//
plugin::PS_amqp::message_bus_publisher::message_bus_publisher(std::string connStr,
							 std::string _exchange,
							 std::string _queue,
							 bool use_ssl,
							 std::string cert_path,
							 std::string client_cert_path,
							 std::string client_key_path ) :
					amqp(connStr, use_ssl, cert_path, client_cert_path, client_key_path),
					exchange(_exchange),
					queue(_queue)
{
	// std::cout << "message_bus_publisher::message_bus_publisher(" << connStr << "," << _exchange << "," << _queue << ")" << std::endl;
}

plugin::PS_amqp::message_bus_publisher::~message_bus_publisher()
{

}

void plugin::PS_amqp::message_bus_publisher::initialize()
{
	if(!exchange.empty()) {
		ex = amqp.createExchange(exchange);

		// Force the disabling of the immediate flag. Yes.. it is misspelled
		// by the authors.
		ex->setParam(~AMQP_IMMIDIATE);

		ex->setHeader("Delivery-mode", 2);
		ex->setHeader("Content-type", "application/json");
		ex->setHeader("Content-encoding", "UTF-8");
	}

}

void plugin::PS_amqp::message_bus_publisher::publish(std::string msg)
{
	// std::cout << "PS_amqp - message_bus_publisher::publish - msg = " << msg << std::endl;

	if(!queue.empty()) {
		ex->Publish(msg, queue);
	}
}
