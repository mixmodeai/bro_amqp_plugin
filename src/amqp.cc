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

#include "config.h"

#include <string>
#include <iostream>
#include <vector>
#include <errno.h>

#include "amqp.h"
#include "threading/MsgThread.h"
#include "threading/formatters/JSON.h"
#include "threading/formatters/Ascii.h"
#include "threading/SerialTypes.h"

#include "amqp.bif.h"

using namespace plugin::PS_amqp;
using namespace logging;
using namespace writer;
using namespace std;

using threading::Value;
using threading::Field;

amqp::amqp(WriterFrontend* frontend) :
		WriterBackend(frontend) {
	json = new threading::formatter::JSON(this,
			threading::formatter::JSON::TS_EPOCH);

	cacert_path = "";
	client_cert_path = "";
	client_key_path = "";
}

amqp::~amqp() {
	if (json) {
		delete json;
	}

	if (message_bus_pub) {
		delete message_bus_pub;
	}
}

std::string amqp::GetTableType(int arg_type, int arg_subtype) {
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
bool amqp::checkError(int code) {
//	if (code != SQLITE_OK && code != SQLITE_DONE) {
//		Error(Fmt("SQLite call failed: %s", sqlite3_errmsg(db)));
//		return true;
//	}

	return false;
}

bool amqp::Init(void)
{
	if(message_bus_connstr.empty() || message_bus_exchange.empty() ||
	   message_bus_queue.empty()   || probeid.empty() ||  envid.empty() ) {
		return false;
	}

	try {

		path = Fmt("\"sensor\": \"probe_%s_%s\",", envid.c_str(), probeid.c_str());

		/*
		if (info_path.length() > 0 ) {
			path = Fmt("\"sensor\": \"probe_%s_%s\", \"log\": \"%s\",",
					envid.c_str(), probeid.c_str(), info_path.c_str());
		} else {

		}
		*/

		message_bus_pub = new plugin::PS_amqp::message_bus_publisher(
											message_bus_connstr, message_bus_exchange, message_bus_queue,
											BifConst::PS_amqp::use_ssl, cacert_path, client_cert_path, client_key_path);
		if (!message_bus_pub) {
			return false;
		}

		message_bus_pub->initialize();

		return true;
	} catch (AMQPException e) {
		MsgThread::Info(Fmt("PS_amqp - Init - AMQPException: %s", e.getMessage().c_str()));
		ReInit();
	} catch (const std::exception &exc) {
		MsgThread::Info(Fmt("PS_amqp - Init - std::exception: %s ", exc.what()));
	    ReInit();
	} catch(...) {
		MsgThread::Info("PS_amqp - Init - Exception found");
		ReInit();
	}

	return false;
}

bool amqp::ReInit(void)
{
	bool ret = false;

	if(message_bus_connstr.empty() || message_bus_exchange.empty() ||
	   message_bus_queue.empty()   || probeid.empty() ||  envid.empty() ) {
		return false;
	}

	try {
		if(message_bus_pub) {
			MsgThread::Info("PS_amqp - Attempt to reinitialize");
			if (message_bus_pub) {
				delete message_bus_pub;
			}
			sleep(AMQP_RETRY_INTERVAL);
			Init();
		}
	} catch(...) {
		MsgThread::Info("PS_amqp - Exception found");
	}

	return true;
}

bool amqp::DoInit(const WriterInfo& info, int arg_num_fields,
		const Field* const * arg_fields)
{
	bool ret = false;

	WriterInfo::config_map::const_iterator it;
	num_fields = arg_num_fields;
	fields = arg_fields;

	try {
		it = info.config.find("connstr");
		if (it == info.config.end()) {
			MsgThread::Info(Fmt("connstr configuration option not found"));
			return false;
		} else {
			message_bus_connstr = it->second;
		}

		it = info.config.find("exchange");
		if (it == info.config.end()) {
			MsgThread::Info(Fmt("exchange configuration option not found"));
			return false;
		} else {
			message_bus_exchange = it->second;
		}

		it = info.config.find("queue");
		if (it == info.config.end()) {
			MsgThread::Info(Fmt("queue configuration option not found"));
			return false;
		} else {
			message_bus_queue = it->second;
		}

		it = info.config.find("probeid");
		if (it == info.config.end()) {
			MsgThread::Info(Fmt("probeid configuration option not found"));
			return false;
		} else {
			probeid = it->second;
		}

		it = info.config.find("envid");
		if (it == info.config.end()) {
			MsgThread::Info(Fmt("envid configuration option not found"));
			return false;
		} else {
			envid = it->second;
		}

		if (BifConst::PS_amqp::use_ssl) {
			it = info.config.find("cacert_path");
			if (it == info.config.end()) {
				MsgThread::Info(Fmt("cacert_path configuration option not found"));
				return false;
			} else {
				cacert_path = it->second;
			}

			it = info.config.find("client_cert_path");
			if (it == info.config.end()) {
				MsgThread::Info(Fmt("client_cert_path configuration option not found"));
				return false;
			} else {
				client_cert_path = it->second;
			}

			it = info.config.find("client_key_path");
			if (it == info.config.end()) {
				MsgThread::Info(Fmt("client_key_path configuration option not found"));
				return false;
			} else {
				client_key_path = it->second;
			}
		}

		info_path = info.path;

		return Init();
	} catch (...) {
		MsgThread::Info("PS_amqp - DoInit - Exception found");
	}

	return false;
}

bool amqp::odesc_to_string_writer(const ODesc &buffer, bool add_log_path) {
	bool ret = false;

	std::string out_buf = std::string(reinterpret_cast<const char*>(buffer.Bytes()));
	out_buf = out_buf.insert(1, path);

	if (add_log_path) {
		out_buf = out_buf.insert(1, Fmt("\"log\": \"%s\",", info_path.c_str()));
	}

	try {
		if (!out_buf.empty()) {
			message_bus_pub->publish(out_buf);
			ret = true;
		}
	} catch (AMQPException e) {
		MsgThread::Info(Fmt("PS_amqp - odesc_to_string_writer(\"%s\") - AMQPException: %s",
							out_buf.c_str(), e.getMessage().c_str()));
		ReInit();
	} catch (...) {
		MsgThread::Info("PS_amqp - odesc_to_string_writer - Exception found");
	}

	return true;
}

bool amqp::DoWrite(int num_fields, const Field* const * fields, Value** vals) {
	try {
		bool add_log_path = true;

		if (message_bus_pub) {
			ODesc buffer;

			for ( int j = 0; j < num_fields; j++ ) {
				const threading::Field* field = fields[j];
				if ( strncasecmp(field->name, "log", 3) == 0 ) {
#if 0
					MsgThread::Info("Found existing \"log\" attribute - using instead of default");
#endif // 0
					add_log_path = false;
					break;
				}
			}

			json->Describe(&buffer, num_fields, fields, vals);
			odesc_to_string_writer(buffer, add_log_path);
		}
	} catch (...) {
		MsgThread::Info("PS_amqp - DoWrite - Exception found");
	}
	return true;
}

bool amqp::DoRotate(const char* rotated_path, double open, double close,
		bool terminating) {
	if (!FinishedRotation("/dev/null", Info().path, open, close, terminating)) {
		Error(Fmt("error rotating %s", Info().path));
		return false;
	}

	return true;
}
