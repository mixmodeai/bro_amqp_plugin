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


#include <string>
#include <iostream>
#include "Plugin.h"
#include "message_bus.h"

namespace plugin {
	namespace PS_amqp {
		Plugin plugin;
	}
}

Plugin::Plugin() {
}

Plugin::~Plugin() {

}

void Plugin::InitPostScript() {
	::plugin::Plugin::InitPostScript();
}

void Plugin::Done() {
	::plugin::Plugin::Done();
}

WriterBackend* Plugin::Instantiate(WriterFrontend* frontend) {
	//std::cout << "AMQP - Plugin::Instantiate(" << frontend << ")" << std::endl;

	return new ::logging::writer::amqp(frontend);
}

plugin::Configuration Plugin::Configure() {
	AddComponent(new ::logging::Component("amqp", Plugin::Instantiate));

	plugin::Configuration config;
	config.name = "PS::amqp";
	config.description = "PacketSled AMQP Plug-in for Bro";
	config.version.major = 1;
	config.version.minor = 5;
	return config;
}
