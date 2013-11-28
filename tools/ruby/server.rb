require 'time'

require './multiclient_tcp_server.rb'

def getTime()
	return Time.now.to_s
end

port=8081
if not ARGV.empty?
  port = ARGV[0]
end

srv = MulticlientTCPServer.new( port, 1, false )

puts "Listening on 127.0.0.1:" + port.to_s

loop do
    if sock = srv.get_socket
        # a message has arrived, it must be read from sock
        header = "---| NEW CONNECTION |------| " + getTime() + " |---"
        puts header
        puts sock.gets( "\r\n" ).chomp( "\r\n" )
        puts "-" * header.length
        puts
    else
        sleep 0.01 # free CPU for other jobs, humans won't notice this latency
    end
end
