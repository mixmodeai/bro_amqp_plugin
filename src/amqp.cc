/*
	This file is part of bro_amqp_plugin.

	Copyright (c) 2015, Packetsled. All rights reserved.

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
// Aaron Eppert - PacketSled - 2015
//#include "config.h"

#include <string>
#include <iostream>
#include <vector>
#include <errno.h>
#include "amqp.h"
#include "threading/SerialTypes.h"

using namespace plugin::PS_amqp;
using namespace logging;
using namespace writer;
using namespace std;

using threading::Value;
using threading::Field;

amqp::amqp(WriterFrontend* frontend) : WriterBackend(frontend) {
	json = new threading::formatter::JSON(this,	threading::formatter::JSON::TS_EPOCH);
}

amqp::~amqp()
{
	if(json) {
		delete json;
	}

	if(message_bus_pub) {
		delete message_bus_pub;
	}
}

std::string amqp::GetTableType(int arg_type, int arg_subtype)
{
	std::string type;

	switch (arg_type) {
		case TYPE_BOOL:
			type = "boolean";
			break;

		case TYPE_INT:
		case TYPE_COUNT:
		case TYPE_COUNTER:
		case TYPE_PORT:
			type = "integer";
			break;

		case TYPE_SUBNET:
		case TYPE_ADDR:
			type = "text";
			break;

		case TYPE_TIME:
		case TYPE_INTERVAL:
		case TYPE_DOUBLE:
			type = "double precision";
			break;

		case TYPE_ENUM:
		case TYPE_STRING:
		case TYPE_FILE:
		case TYPE_FUNC:
			type = "text";
			break;

		case TYPE_TABLE:
		case TYPE_VECTOR:
			type = "text";
			break;

		default:
			type = Fmt("%d", arg_type);
	}

	return type;
}

// returns true true in case of error
bool amqp::checkError(int code)
{
//	if (code != SQLITE_OK && code != SQLITE_DONE) {
//		Error(Fmt("SQLite call failed: %s", sqlite3_errmsg(db)));
//		return true;
//	}

	return false;
}

bool amqp::DoInit(const WriterInfo& info, int arg_num_fields, const Field* const * arg_fields)
{
	bool ret = false;

	WriterInfo::config_map::const_iterator it;
	num_fields = arg_num_fields;
	fields = arg_fields;
	std::string message_bus_connstr;
	std::string message_bus_exchange;
	std::string message_bus_queue;

	do {
		it = info.config.find("connstr");
		if (it == info.config.end()) {
			MsgThread::Info(Fmt("connstr configuration option not found"));
			break;
		} else {
			message_bus_connstr = it->second;
			std::cout << "message_bus_connstr = " << message_bus_connstr << std::endl;
		}

		it = info.config.find("exchange");
		if (it == info.config.end()) {
			MsgThread::Info(Fmt("exchange configuration option not found"));
			break;
		} else {
			message_bus_exchange = it->second;
			std::cout << "message_bus_exchange = " << message_bus_exchange << std::endl;
		}

		it = info.config.find("queue");
		if (it == info.config.end()) {
			MsgThread::Info(Fmt("queue configuration option not found"));
			break;
		} else {
			message_bus_queue = it->second;
			std::cout << "message_bus_queue = " << message_bus_queue << std::endl;
		}

		std::cout << "pre-new plugin::PS_amqp::message_bus_publisher" << std::endl;

		message_bus_pub = new plugin::PS_amqp::message_bus_publisher(message_bus_connstr,
																	 message_bus_exchange,
																	 message_bus_queue);
		std::cout << "post-new plugin::PS_amqp::message_bus_publisher" << std::endl;

		if(!message_bus_pub) {
			break;
		}

		message_bus_pub->initialize();
		ret = true;
	} while(0);

	return ret;
}

bool amqp::odesc_to_string_writer(const ODesc &buffer)
{
	bool ret = false;
	std::string out_buf = std::string(reinterpret_cast<const char*>(buffer.Bytes()));

	if(!out_buf.empty()) {
		message_bus_pub->publish(out_buf);
		ret = true;
	}

	return true;
}

bool amqp::DoWrite(int num_fields, const Field* const * fields, Value** vals)
{
	if (message_bus_pub) {
		ODesc buffer;
		json->Describe(&buffer, num_fields, fields, vals);
		odesc_to_string_writer(buffer);
	}
	return true;
}

bool amqp::DoRotate(const char* rotated_path, double open, double close, bool terminating)
{
	if (!FinishedRotation("/dev/null", Info().path, open, close, terminating)) {
		Error(Fmt("error rotating %s", Info().path));
		return false;
	}

	return true;
}
