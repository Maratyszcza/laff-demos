function PerfPlot(root, title, is_dgemm) {
	this.data = [];
	this.performanceTop = is_dgemm ? 5 : 50;

	var margin = {top: 75, right: 25, bottom: 75, left: 75};
	this.width = 1024 - margin.left - margin.right;
	this.height = 768 - margin.top - margin.bottom;

	this.scaleX = d3.scale.log();
	if (is_dgemm) {
		this.scaleX
			.base(10)
			.domain([10, 1000])
			.range([0, this.width]);
	} else {
		this.scaleX
			.base(2)
			.domain([64, 1024*1024*64])
			.range([0, this.width]);
	}

	this.scaleY = d3.scale.linear()
		.domain([0, this.performanceTop])
		.range([ this.height, 0 ]);

	this.chart = d3.select(root)
		.append('svg:svg')
		.attr('width', this.width + margin.right + margin.left)
		.attr('height', this.height + margin.top + margin.bottom)
		.attr('class', 'chart')

	this.main = this.chart.append('g')
		.attr('transform', 'translate(' + margin.left + ',' + margin.top + ')')
		.attr('width', this.width)
		.attr('height', this.height)

	var tickMap = new Array();
	tickMap[64] = "64";
	tickMap[1024] = "1K";
	tickMap[16384] = "16K";
	tickMap[256*1024] = "256K";
	tickMap[4*1024*1024] = "4M";
	tickMap[64*1024*1024] = "64M";
	this.axisX = d3.svg.axis()
		.scale(this.scaleX)
		.orient('bottom');
	if (is_dgemm) {
		this.axisX.tickFormat(this.scaleX.tickFormat(10, ".f"));
	} else {
		this.axisX
			.tickValues([64, 1024, 16384, 256*1024, 4*1024*1024, 64*1024*1024])
			.tickFormat(function(d) { return tickMap[d]; });
	}

	this.axisY = d3.svg.axis()
		.scale(this.scaleY)
		.orient('left');

	this.main.append('g')
		.attr('transform', 'translate(0,' + this.height + ')')
		.attr('class', 'x axis')
		.call(this.axisX)
			.append("text")
			.attr("class", "label")
			.attr("x", this.width / 2)
			.attr("y", margin.bottom * 0.60)
			.style("text-anchor", "middle")
			.text(is_dgemm ? "Problem size (n = m = k)" : "Array size (bytes)");

	this.main.append('g')
		.attr('transform', 'translate(0,0)')
		.attr('class', 'y axis')
		.call(this.axisY)
			.append("text")
			.attr("class", "label")
			.attr("transform", "rotate(-90)")
			.attr("x", -this.height / 2)
			.attr("y", -margin.left * 0.60)
			.style("text-anchor", "middle")
			.text(is_dgemm ? "Performance, GFLOPS" : "Latency, nSec");

	var scaleX = this.scaleX;
	var scaleY = this.scaleY;
	this.line = d3.svg.line()
		.x(function(d) { return scaleX(d.size); })
		.y(function(d) { return scaleY(d.performance); })
		.interpolate("linear");

	this.chart.append("text")
		.attr("x", margin.left + this.width / 2)
		.attr("y", margin.top * 0.75)
		.attr("text-anchor", "middle")
		.attr("class", "title")
		.text(title);
	if (is_dgemm) {
		this.main.append("svg:path")
			.datum([])
			.attr("class", "naive trend");
		this.main.append("svg:path")
			.datum([])
			.attr("class", "blocked trend");
		this.main.append("svg:path")
			.datum([])
			.attr("class", "blis trend");
	} else {
		this.main.append("svg:path")
			.datum([])
			.attr("class", "latency trend");
	}

	if (is_dgemm) {
		var explainationMap = {
			"Naive": "Naive implementation of matrix-matrix multiplication performs well when matrices are small. However, performance on big matrices is not as good: big matrices do not fit into cache and processor needs to access slow memory during computations.",
			"Blocked": "Blocked implementation of matrix-matrix multiplication performs operations on blocks of elements instead of individual elements. Multiplication of two blocks is essentially a naive matrix-matrix multiplication and blocks are small enough to fit into cache. That is why blocked matrix-matrix multiplication outperforms naive implementation on large matrix sizes.",
			"BLIS": "Blocking is an important, but not the only possible optimization in matrix-matrix implementation. High-performance linear algebra libraries, such as BLIS, utilize additional optimizations, which let them improve upon blocked implementation."
		};

		this.main.append("text")
			.attr("x", 150)
			.attr("y", 50)
			.attr("class", "wrap")
			.attr("id", "explaination");

		var setExplaination = function(text) {
			if (!text) {
				d3.select("#explaination").selectAll("tspan").remove();
			} else {
				d3plus.textwrap()
				.container(d3.select("#explaination"))
				.shape("square")
				.width(600)
				.text(text)
				.draw();
    		}
    	}

		var legend = this.main.selectAll("text.legend").data([
			{benchmark: "Naive", name: "Naive"},
			{benchmark: "Blocked", name: "Blocked"},
			{benchmark: "BLIS", name: "BLIS"}
		]).enter();
		legend.append("text")
					.attr("x", function(d, i) { return 30; })
					.attr("y", function(d, i) { return 50 + i * 20; })
					.attr("class", function(d) { return "legend " + d.benchmark.toLowerCase(); })
					.text(function(d) { return d.name; })
					.on("mouseover", function(d) { setExplaination(explainationMap[d.name]); })
					.on("mouseout", function() { setExplaination(null); })
					.on("touchstart", function(d) { setExplaination(explainationMap[d.name]); })
					.on("touchleave", function() { setExplaination(null); });
		legend.append("rect")
					.attr("x", function(d, i) { return 15; })
					.attr("y", function(d, i) { return 40 + i * 20; })
					.attr("width", 10)
					.attr("height", 10)
					.attr("class", function(d) { return "legend " + d.benchmark.toLowerCase(); })
					.on("mouseover", function(d) { setExplaination(explainationMap[d.name]); })
					.on("mouseout", function() { setExplaination(null); })
					.on("touchstart", function(d) { setExplaination(explainationMap[d.name]); })
					.on("touchleave", function() { setExplaination(null); });
	}
}

PerfPlot.prototype.add = function(benchmark, size, performance) {
	if (isFinite(performance)) {
		var scaleX = this.scaleX;
		var scaleY = this.scaleY;
		if (performance > plot.performanceTop) {
			this.performanceTop = Math.ceil(performance / 0.5) * 0.5;
			this.scaleY.domain([0, this.performanceTop]);
			this.chart.selectAll("g.y.axis").call(this.axisY);
			if (this.data.length !== 0) {
				this.main.selectAll("circle")
					.attr("cy", function(d) { return scaleY(d.performance); });
				this.main.selectAll("path.trend")
					.attr("d", this.line);
			}
		}
		this.data.push({
			size: size,
			benchmark: benchmark,
			performance: performance
		});
		var subdata = this.data.filter(function(d) { return d.benchmark === benchmark; });
		this.main.selectAll("circle." + benchmark.toLowerCase())
			.data(subdata)
			.enter()
				.append("circle")
				.attr("cx", function(d) { return scaleX(d.size); })
				.attr("cy", function(d) { return scaleY(d.performance); })
				.attr("class", benchmark.toLowerCase())
				.attr("r", 2);
		this.main.selectAll("path.trend." + benchmark.toLowerCase())
			.datum(subdata)
			.attr("d", this.line);
	}
}
