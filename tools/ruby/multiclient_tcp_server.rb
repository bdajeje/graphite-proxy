require 'socket'

class MulticlientTCPServer

    # A nonblocking TCP server
    # - that serves several clients
    # - that is very efficient thanks to the 'select' system call
    # - that does _not_ use Ruby threads

    def initialize( port, timeout, verbose )
        @port = port        # the server listens on this port
        @timeout = timeout  # in seconds
        @verbose = verbose  # a boolean
        @connections = []
        @server =
            begin
                TCPServer.new( @port )
            rescue SystemCallError => ex
                raise "cannot initialize tcp server for port #{@port}: #{ex}"
            end
    end

    def get_socket
        # Process incoming connections and messages.

        # When a message has arrived, we return the connection's TcpSocket.
        # Applications can read from this socket with gets(),
        # and they can respond with write().

        # one select call for three different purposes -> saves timeouts
        ios = select( [@server]+@connections, nil, @connections, @timeout ) or
            return nil
        # disconnect any clients with errors
        ios[2].each do |sock|
            sock.close
            @connections.delete( sock )
            raise "socket #{sock.peeraddr.join(':')} had error"
        end
        # accept new clients
        ios[0].each do |s|
            # loop runs over server and connections; here we look for the former
            s==@server or next
            client = @server.accept or
                raise "server: incoming connection, but no client"
            @connections << client
            @verbose and
                puts "server: incoming connection no. #{@connections.size} from #{client.peeraddr.join(':')}"
            # give the new connection a chance to be immediately served
            ios = select( @connections, nil, nil, @timeout )
        end
        # process input from existing client
        ios[0].each do |s|
            # loop runs over server and connections; here we look for the latter
            s==@server and next
            # since s is an element of @connections, it is a client created
            # by @server.accept, hence a TcpSocket < IPSocket < BaseSocket
            if s.eof?
                # client has closed connection
                @verbose and
                    puts "server: client closed #{s.peeraddr.join(':')}"
                @connections.delete(s)
                next
            end
            @verbose and
                puts "server: incoming message from #{s.peeraddr.join(':')}"
            return s # message can be read from this
        end
        return nil # no message arrived
    end

end # class MulticlientTCPServer