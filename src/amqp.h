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
//
// Log writer for AMQP logs.
//

#ifndef LOGGING_WRITER_AMQP_H
#define LOGGING_WRITER_AMQP_H

//#include "config.h"

#include <string>
#include "logging/WriterBackend.h"
#include "threading/formatters/JSON.h"
#include "message_bus.h"

using namespace std;

namespace logging {
	namespace writer {
		class amqp : public WriterBackend {
			public:
				amqp(WriterFrontend* frontend);
				~amqp();

				static WriterBackend* Instantiate(WriterFrontend* frontend)	{
					return new amqp(frontend);
				}

			protected:
				virtual bool DoInit(const WriterInfo& info, int arg_num_fields, const threading::Field* const* arg_fields);
				virtual bool DoWrite(int num_fields, const threading::Field* const* fields, threading::Value** vals);
				virtual bool DoSetBuf(bool enabled) { return true; }
				virtual bool DoRotate(const char* rotated_path, double open, double close, bool terminating);
				virtual bool DoFlush(double network_time)	{ return true; }
				virtual bool DoFinish(double network_time)	{ return true; }
				virtual bool DoHeartbeat(double network_time, double current_time)	{ return true; }

			private:
				bool checkError(int code);

				int AddParams(threading::Value* val, int pos);
				std::string GetTableType(int, int);

				const threading::Field* const *fields; // raw mapping
				unsigned int num_fields;

				plugin::PS_amqp::message_bus_publisher *message_bus_pub;
				threading::formatter::JSON *json;

				bool odesc_to_string_writer(const ODesc &buffer);

		};
	}
}

#endif // LOGGING_WRITER_AMQP_H
