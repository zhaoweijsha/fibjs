var test = require("test");
test.setup();

var net = require('net');
var fs = require('fs');
var os = require('os');
var coroutine = require('coroutine');

var base_port = coroutine.vmid * 10000;

var net_config = {
	family: net.AF_INET6,
	address: '::1',
	host: '[::1]'
};

if (os.type == 'Windows' && os.version < "6.0")
	net_config = {
		family: net.AF_INET,
		address: '127.0.0.1',
		host: '127.0.0.1'
	};

var backend = {
	"Windows": "IOCP",
	"Darwin": "KQueue",
	"FreeBSD": "KQueue",
	"Linux": "EPoll"
}[os.type];


function del(f) {
	try {
		fs.unlink(f);
	} catch (e) {}
}

describe("net", function() {
	it("backend", function() {
		assert.equal(net.backend(), backend);
	});

	var ss = [];

	after(function() {
		ss.forEach(function(s) {
			s.close();
		});
	});

	it("echo", function() {
		function connect(c) {
			console.log(c.remoteAddress, c.remotePort, "->",
				c.localAddress, c.localPort);
			try {
				var b;

				while (b = c.recv())
					c.send(b);
			} finally {
				c.close();
				c.dispose();
			}
		}

		function accept(s) {
			while (1)
				coroutine.start(connect, s.accept());
		}

		var s = new net.Socket(net_config.family,
			net.SOCK_STREAM);
		ss.push(s);

		s.bind(8080 + base_port);
		s.listen();
		coroutine.start(accept, s);

		function conn_socket() {
			var s1 = new net.Socket(net_config.family, net.SOCK_STREAM);
			s1.connect(net_config.address, 8080 + base_port);
			console.log(s1.remoteAddress, s1.remotePort, "<-",
				s1.localAddress, s1.localPort);
			s1.send(new Buffer("GET / HTTP/1.0"));
			assert.equal("GET / HTTP/1.0", s1.recv());
			s1.close();
			s1.dispose();
		}

		function conn() {
			var s1 = net.connect(net_config.address, 8080 + base_port, net_config.family);
			console.log(s1.remoteAddress, s1.remotePort, "<-",
				s1.localAddress, s1.localPort);
			s1.send(new Buffer("GET / HTTP/1.0"));
			assert.equal("GET / HTTP/1.0", s1.recv());
			s1.close();
			s1.dispose();
		}

		function conn_url() {
			var s1 = net.connect('tcp://' + net_config.host + ':' + (8080 + base_port));
			console.log(s1.remoteAddress, s1.remotePort, "<-",
				s1.localAddress, s1.localPort);
			s1.send(new Buffer("GET / HTTP/1.0"));
			assert.equal("GET / HTTP/1.0", s1.recv());
			s1.close();
			s1.dispose();
		}

		conn_socket();
		conn();
		conn_url();
	});

	it("copyTo", function() {
		var str = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";

		for (var i = 0; i < 8; i++)
			str = str + str;

		function accept1(s) {
			while (true) {
				var c = s.accept();

				// c.write(str);

				fs.writeFile('net_temp_000001' + base_port, str);
				var f = fs.open('net_temp_000001' + base_port);
				assert.equal(f.copyTo(c), str.length);
				f.close();
				c.close();
			}
		}

		var s1 = new net.Socket(net_config.family, net.SOCK_STREAM);
		ss.push(s1);

		s1.bind(8081 + base_port);
		s1.listen();
		coroutine.start(accept1, s1);

		function t_conn() {
			var c1 = new net.Socket();
			c1.connect('127.0.0.1', 8081 + base_port);

			var f1 = fs.open('net_temp_000002' + base_port, 'w');
			assert.equal(c1.copyTo(f1), str.length);
			c1.close();
			f1.close();

			assert.equal(str, fs.readFile('net_temp_000002' + base_port));
		}

		for (var i = 0; i < 100; i++)
			t_conn();

		for (var i = 0; i < 10; i++)
			str = str + str;

		t_conn();

		del('net_temp_000001' + base_port);
		del('net_temp_000002' + base_port);
	});

	it("read & recv", function() {
		function accept2(s) {
			while (true) {
				var c = s.accept();

				c.write('a');
				coroutine.sleep(100);
				c.write('a');
				coroutine.sleep(100);
				c.write('b');
				coroutine.sleep(100);
				c.write('c');
				coroutine.sleep(100);
				c.write('d');
				coroutine.sleep(100);

				c.close();
			}
		}

		var s2 = new net.Socket(net_config.family, net.SOCK_STREAM);
		ss.push(s2);

		s2.bind(8082 + base_port);
		s2.listen();
		coroutine.start(accept2, s2);

		var c1 = new net.Socket();
		c1.connect('127.0.0.1', 8082 + base_port);
		assert.equal('a', c1.recv(100));
		assert.equal('ab', c1.read(2));
		assert.equal('c', c1.read(1));
		assert.equal('d', c1.read(3));
	});

	it("re-entrant", function() {
		function accept2(s) {
			while (true) {
				s.accept();
			}
		}

		var s2 = new net.Socket(net_config.family, net.SOCK_STREAM);
		ss.push(s2);

		s2.bind(8083 + base_port);
		s2.listen();
		coroutine.start(accept2, s2);

		coroutine.sleep(10);
		assert.throws(function() {
			s2.accept();
		});

		function recv2(s) {
			s.recv();
		}

		var c1 = new net.Socket();
		c1.connect('127.0.0.1', 8083 + base_port);
		coroutine.start(recv2, c1);

		coroutine.sleep(10);
		assert.throws(function() {
			c1.recv();
		});

		function send2(s) {
			var b = new Buffer("aaaaaaaa");
			b.resize(10240);
			while (true)
				s.send(b);
		}

		var c1 = new net.Socket();
		c1.connect('127.0.0.1', 8083 + base_port);
		coroutine.start(send2, c1);

		coroutine.sleep(100);
		assert.throws(function() {
			var b = new Buffer("aaaaaaaa");
			c1.send(b);
		});

		function accept3(s) {
			var c = s.accept();
			c.send(c.recv());
		}

		var s3 = new net.Socket(net_config.family, net.SOCK_STREAM);
		s3.bind(8084 + base_port);
		s3.listen();
		coroutine.start(accept3, s3);

		var st = 0;

		function send3(s) {
			st = 1;
			coroutine.sleep(100);
			s.send(new Buffer("aaa"));
		}

		var c1 = new net.Socket();
		c1.connect('127.0.0.1', 8084 + base_port);
		coroutine.start(send3, c1);

		assert.equal(st, 0);
		assert.equal(c1.recv().toString(), "aaa");
		assert.equal(st, 1);
	});

	it("bind same port", function() {
		new net.TcpServer(8811 + base_port, function(c) {});
		assert.throws(function() {
			new net.TcpServer(8811 + base_port, function(c) {});
		});
	});

	it("stats", function() {
		var svr = new net.TcpServer(8812 + base_port, function(c) {
			var d;
			while (d = c.read(100))
				c.write(d);
		});

		ss.push(svr.socket);
		svr.asyncRun();

		assert.deepEqual({
			"total": 0,
			"connections": 0,
			"accept": 0,
			"close": 0
		}, svr.stats.toJSON());

		var c1 = new net.Socket();
		c1.connect('127.0.0.1', 8812 + base_port);

		coroutine.sleep(10);
		assert.deepEqual({
			"total": 1,
			"connections": 1,
			"accept": 1,
			"close": 0
		}, svr.stats.toJSON());

		svr.stats.reset();
		assert.deepEqual({
			"total": 1,
			"connections": 1,
			"accept": 0,
			"close": 0
		}, svr.stats.toJSON());

		c1.close();

		svr.stats.reset();
		coroutine.sleep(10);
		assert.deepEqual({
			"total": 1,
			"connections": 0,
			"accept": 0,
			"close": 1
		}, svr.stats.toJSON());
	});

	describe("abort Pending I/O", function() {
		function close_it(s) {
			coroutine.sleep(50);
			s.close();
		}

		it("abort connect", function() {
			var c1 = new net.Socket();
			coroutine.start(close_it, c1);
			assert.throws(function() {
				c1.connect('12.0.0.1', 8083 + base_port);
			});
		});

		it("abort accept", function() {
			var c1 = new net.Socket();
			c1.bind(8180 + base_port);
			c1.listen();

			coroutine.start(close_it, c1);

			assert.throws(function() {
				c1.accept();
			});
		});

		it("abort read", function() {
			var c1 = new net.Socket();
			c1.connect('127.0.0.1', 8080 + base_port);
			coroutine.start(close_it, c1);
			assert.throws(function() {
				c1.read();
			});
		});
	});

	it("Memory Leak detect", function() {
		var ss, no1;
		GC();
		coroutine.sleep(100);
		GC();
		no1 = os.memoryUsage().nativeObjects.objects;

		ss = new net.TcpServer(9812, function(c) {});
		coroutine.start(function() {
			ss.run();
		});

		coroutine.sleep(50);
		ss.stop();
		ss = undefined;
		coroutine.sleep(50);

		GC();
		assert.equal(no1, os.memoryUsage().nativeObjects.objects);

		ss = new net.TcpServer(9813, function(c) {});
		ss.asyncRun();

		coroutine.sleep(50);
		ss.stop();
		ss = undefined;
		coroutine.sleep(50);

		GC();
		assert.equal(no1, os.memoryUsage().nativeObjects.objects);

		(function() {
			var s = new net.TcpServer(9884, function() {});
		})();

		coroutine.sleep(50);

		GC();
		assert.equal(no1, os.memoryUsage().nativeObjects.objects);
	});

	xdescribe("Smtp", function() {
		var s;

		it("new & connect", function() {
			s = new net.Smtp();
			s.connect("smtp.ym.163.com", 25);
			s.socket.close();
		});

		it("net.openSmtp", function() {
			s = net.openSmtp("smtp.exmail.qq.com", 25);
		});

		it("command", function() {
			assert.equal(s.command("HELO", "baoz.me").substr(0, 4),
				"250 ");

			assert.throws(function() {
				s.command("FUCK", "baoz.me");
			});
		});

		it("hello", function() {
			s.hello();
		});

		xdescribe("Auth", function() {
			it("login", function() {
				s.login("lion@baoz.cn", "");
			});

			it("from", function() {
				s.from("lion@baoz.cn");
			});

			it("to", function() {
				s.to("lion@baoz.cn");
			});

			it("data", function() {
				s.data("from:lion@baoz.cn\r\n" + "to:lion@baoz.cn\r\n" + "subject:test title\r\n\r\n" + "test text");
			});
		});

		it("quit", function() {
			s.quit();
		});
	});
});

//test.run(console.DEBUG);