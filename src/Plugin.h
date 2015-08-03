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
#ifndef BRO_PLUGIN_PS_AMQP
#define BRO_PLUGIN_PS_AMQP

#include "logging/WriterBackend.h"
#include <plugin/Plugin.h>
#include <DebugLogger.h>
#include "amqp.h"
#include "message_bus.h"

using namespace plugin::PS_amqp;
using namespace logging;

namespace plugin {
	namespace PS_amqp {

	class Plugin : public ::plugin::Plugin {
			public:
				Plugin();
				~Plugin();
				static WriterBackend* Instantiate(WriterFrontend* frontend);
				virtual void InitPostScript();
				virtual void Done();
			protected:
				// Overridden from plugin::Plugin.
				virtual plugin::Configuration Configure();
			private:
		};

		extern Plugin plugin;
	}
}
#endif
