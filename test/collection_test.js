var test = require("test");
test.setup();

var collection = require('collection');
var os = require('os');

describe("collection", function() {
	describe('List', function() {
		function isBigEnough(element, index) {
			return (element >= 10);
		}

		it("create by array", function() {
			var a = new collection.List([1, 2, 3]);
			assert.deepEqual(a.toArray(), [1, 2, 3]);
		});


		it("pop empty list", function() {
			var a = new collection.List();
			assert.equal(a.pop(), null);
		});

		it("resize", function() {
			var a = new collection.List();
			a.resize(256);
			assert.equal(a.length, 256);
			assert.isUndefined(a[0]);
		});

		it("index", function() {
			var a = new collection.List();
			a.resize(256);
			for (var i = 0; i < 256; i++) {
				a[i] = 256 - i;
				assert.equal(a[i], 256 - i);
			}
			a.resize(0);
		});

		it("Memory Leak detect", function() {
			var a = new collection.List();
			GC();
			var no1 = os.memoryUsage().nativeObjects.objects;
			a.push(new Buffer());
			GC();
			assert.equal(no1 + 1, os.memoryUsage().nativeObjects.objects);
			a.resize(0);
			GC();
			assert.equal(no1, os.memoryUsage().nativeObjects.objects);
			new Buffer();
			assert.equal(no1 + 1, os.memoryUsage().nativeObjects.objects);
			GC();
			assert.equal(no1, os.memoryUsage().nativeObjects.objects);
		});

		it("toArray", function() {
			var a = new collection.List();
			a.push(1);
			assert.deepEqual(a.toArray(), [1]);
		});

		it("push", function() {
			var a = new collection.List([1]);
			a.push(2, 3, 4);
			assert.deepEqual(a.toArray(), [1, 2, 3, 4]);
		});

		it("pushArray", function() {
			var a = new collection.List([1]);
			a.pushArray([2, 3, 4]);
			assert.deepEqual(a.toArray(), [1, 2, 3, 4]);
		});

		it("pop", function() {
			var a = new collection.List([1, 2, 3, 4]);
			assert.equal(a.pop(), 4);
			assert.equal(a.length, 3);
		});

		it("slice", function() {
			var a = new collection.List([1, 2, 3]);
			assert.deepEqual(a.slice(1).toArray(), [2, 3]);
			assert.deepEqual(a.slice(1, 2).toArray(), [2]);
			assert.deepEqual(a.slice(1, 0).toArray(), []);
		});

		it("concat", function() {
			var a = new collection.List([1, 2, 3]);
			assert.deepEqual(a.concat(a).toArray(), [1, 2, 3, 1, 2, 3]);
			assert.deepEqual(a.concat(new collection.List()).toArray(), [1, 2,
				3
			]);
			assert.deepEqual(a.concat([4, 5, 6]).toArray(), [1, 2, 3, 4, 5,
				6
			]);
			assert.deepEqual(a.concat(a, [4, 5, 6], a).toArray(), [1, 2, 3,
				1, 2, 3, 4, 5, 6, 1, 2, 3
			]);
		});

		it("every", function() {
			var passed = new collection.List();
			passed.push(12, 5, 8, 130, 44);
			assert.equal(passed.every(isBigEnough), false);

			passed = new collection.List();
			passed.push(12, 54, 18, 130, 44);
			assert.equal(passed.every(isBigEnough), true);
		});

		it("some", function() {
			var passed = new collection.List();
			passed.push(12, 5, 8, 130, 44);
			assert.ok(passed.some(isBigEnough));

			passed = new collection.List();
			passed.push(1, 5, 8, 3, 4);
			assert.notOk(passed.some(isBigEnough));
		});

		it("filter", function() {
			var passed = new collection.List();
			passed.push(12, 5, 8, 130, 44);
			assert.deepEqual(passed.filter(isBigEnough).toArray(), [12, 130,
				44
			]);
		});

		it("map", function() {
			var passed = new collection.List();
			passed.push(1, 4, 9);
			assert.deepEqual(passed.map(Math.sqrt).toArray(), [1, 2, 3]);
		});

		it("forEach", function() {
			var passed = new collection.List();
			passed.push(1, 4, 9);
			passed.forEach(function(v, i) {
				assert.equal(passed[i], v);
			});
		});

		it("freeze", function() {
			var passed = new collection.List();
			passed.push(1, 4, 9);
			passed.freeze();

			assert.throws(function() {
				passed[0] = 100;
			});

			assert.throws(function() {
				passed.resize(10);
			});

			assert.throws(function() {
				passed.push(10);
			});

			assert.throws(function() {
				passed.pop();
			});

			assert.throws(function() {
				passed.concat([4, 5, 6]);
			});
		});

	});

	describe("Queue", function() {
		it("add", function() {
			var q = new collection.Queue(3);

			assert.equal(q.add(100), true);
			assert.equal(q.add(200), true);
			assert.equal(q.add(300), true);
			assert.deepEqual(q.toArray(), [100, 200, 300]);

			assert.throws(function() {
				q.add(400);
			});
			assert.deepEqual(q.toArray(), [100, 200, 300]);
		});

		it("offer", function() {
			var q = new collection.Queue(3);

			assert.equal(q.offer(100), true);
			assert.equal(q.offer(200), true);
			assert.equal(q.offer(300), true);
			assert.deepEqual(q.toArray(), [100, 200, 300]);

			assert.equal(q.offer(400), false);
			assert.deepEqual(q.toArray(), [100, 200, 300]);
		});

		it("remove", function() {
			var q = new collection.Queue(3);

			q.add(100);
			q.add(200);
			assert.equal(q.remove(), 100);
			assert.deepEqual(q.toArray(), [200]);

			assert.equal(q.remove(), 200);
			assert.deepEqual(q.toArray(), []);

			assert.throws(function() {
				q.remove();
			});
			assert.deepEqual(q.toArray(), []);
		});

		it("poll", function() {
			var q = new collection.Queue(3);

			q.add(100);
			q.add(200);
			assert.equal(q.poll(), 100);
			assert.deepEqual(q.toArray(), [200]);

			assert.equal(q.poll(), 200);
			assert.deepEqual(q.toArray(), []);

			assert.isNull(q.poll());
			assert.deepEqual(q.toArray(), []);
		});

		it("clear/length", function() {
			var q = new collection.Queue(3);

			q.add(100);
			q.add(200);
			assert.deepEqual(q.toArray(), [100, 200]);

			q.clear();
			assert.deepEqual(q.toArray(), []);
			assert.equal(q.length, 0);
		});

		it("element", function() {
			var q = new collection.Queue(3);

			q.add(100);
			q.add(200);
			assert.equal(q.element(), 100);
			assert.deepEqual(q.toArray(), [100, 200]);

			q.clear();
			assert.throws(function() {
				q.element();
			});
		});

		it("peek", function() {
			var q = new collection.Queue(3);

			q.add(100);
			q.add(200);
			assert.equal(q.element(), 100);
			assert.deepEqual(q.toArray(), [100, 200]);

			q.clear();
			assert.isNull(q.peek());
		});
	});
});

// test.run(console.DEBUG);