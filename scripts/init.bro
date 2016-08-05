@load base/frameworks/cluster
@load policy/misc/loaded-scripts
@load base/frameworks/communication

module PS_amqp;

export {
        ## RabbitMQ Connection String - <password>:<user>@<IP>:<port>
        const connstr = "" &redef;
        ## RabbitMQ Exchange
        const exchange = "yourorg.direct" &redef;
        ## RabbitMQ Queue
        const queue = "bro" &redef;
        ## Sensor identifier
        const probeid = 0 &redef;
        ## Environment identifier
        const envid = 0 &redef;
        ## Enabled flag
        const enabled = T &redef;
        ## Use amqp as transport
        const use_ssl = F &redef;
        ## CA Cert Path
        const cacert_path = "/opt/yourorg/etc/tls/rmq/ca/cacert.pem" &redef;
        ## Client Cert Path
        const client_cert_path = "/opt/yourorg/etc/tls/rmq/client/cert.pem" &redef;
        ## Client Key Path
        const client_key_path = "/opt/yourorg/etc/tls/rmq/client/rabbitmq.pem" &redef;
}

redef PS_amqp::enabled = ( Cluster::local_node_type() == Cluster::MANAGER || Cluster::local_node_type() == Cluster::NONE );

# event bro_init() &priority=-5
#         {
#         Log::disable_stream(Communication::LOG);
#         for ( sid in Log::active_streams )
#                 {
#                 if( sid !in excluded_log_ids ) {
#                         print sid;
#                         local filt: Log::Filter = [$name = "ps-amqp-filter",
#                                                         $config=table(
#                                                         ["probeid"]  = fmt("%s", probeid),
#                                                         ["envid"]  = fmt("%s", envid),
#                                                         ["connstr"]  = connstr,
#                                                         ["exchange"] = exchange,
#                                                         ["queue"]    = fmt("probe_%d_%d", envid, probeid)),
#                                                         $writer = Log::WRITER_AMQP];
#                         Log::add_filter(sid, filt);
#                         if(!logfiles)
#                                 {
#                                 Log::remove_filter(sid, "default");
#                                 }
#                         }
#                 }
#         }
