
module amqp_test;

export {
	type Info: record {
		uid:     string  &log;
  		ts:		 time    &log;
  };

  redef enum Log::ID += { LOG };

  # Define a hook event. By convention, this is called
  # "log_<stream>".
  global log_amqp_test: event(rec: Info);
}

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

event new_connection(c: connection) {
  local tmp: Info = [	$uid=c$uid,
						$ts=c$start_time ];

  Log::write(amqp_test::LOG, tmp);
}
