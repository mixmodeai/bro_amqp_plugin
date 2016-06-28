![PacketSled Logo](https://packetsled.com/wp-content/themes/freshbiz/img/packetsled-logo.png)
# PS::amqp - Bro Writer Plugin to Stream Data to an AMQP Server


## Introduction
PS::amp is a plugin that provides support for writing data to an ``AMQP`` server.
This plugin provides support the ``AMQP Protocol,`` specifically ``RabbitMQ.``


## Prerequisites

1. AMQPCPP - ``https://github.com/akalend/amqpcpp``
1. RabbitMQ-C - ``https://github.com/alanxz/rabbitmq-c``


## Compilation and Installation

    cd <path to>/bro_amqp_plugin
    ./configure --bro-dist=$PATH_TO_SRC_TREE_FOR/bro
    make -j10
    make install

## Usage

Using PS::amqp requires instantiating a Bro Log Filter as shown below. From there, a write to the created log will cause the plugin to send the data to the AMQP server.

**Note**: "password" and "username" are in the appropriate order per the underlying libraries being used.

```bro
event bro_init() {
             local filter: Log::Filter = [
		$name="amqp",
		$config=table(["connstr"]  = "password:username@localhost:5672",
                  ["exchange"] = "our.direct",
                  ["queue"]    = "queue"),
		$writer=Log::WRITER_AMQP
	];

	Log::create_stream(amqp_test::LOG, [$columns=Info, $ev=log_amqp_test]);
	Log::add_filter(amqp_test::LOG, filter);
	Log::remove_filter(amqp_test::LOG, "default");
}
```

## License

GPL

Copyright (c) 2015, Packetsled. All rights reserved.

PS::amqp is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

PS::amqp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with PS::amqp.  If not, see <http://www.gnu.org/licenses/>.
