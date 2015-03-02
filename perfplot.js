function PerfPlot(root, title, is_dgemm) {
	this.data = [];
	this.performanceTop = is_dgemm ? 5 : 50;

	var margin = {top: 75, right: 25, bottom: 75, left: 75};
	this.width = 800 - margin.left - margin.right;
	this.height = 600 - margin.top - margin.bottom;

	this.scaleX = d3.scale.log();
	if (is_dgemm) {
		this.scaleX
			.base(10)
			.domain([10, 1000])
			.range([0, this.width]);
	} else {
		this.scaleX
			.base(2)
			.domain([16, 1024*1024*16])
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
	tickMap[16] = "16";
	tickMap[16*32] = "512";
	tickMap[16*1024] = "16K";
	tickMap[16*32*1024] = "512K";
	tickMap[16*1024*1024] = "16M";
	this.axisX = d3.svg.axis()
		.scale(this.scaleX)
		.orient('bottom');
	if (is_dgemm) {
		this.axisX.tickFormat(this.scaleX.tickFormat(10, ".f"));
	} else {
		this.axisX
			.tickValues([16, 16*32, 16*1024, 16*32*1024, 16*1024*1024])
			.tickFormat(function(d) { console.log(d); return tickMap[d]; });
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
			.text(is_dgemm ? "Problem size (n = m = k)" : "Array size (32-bit words)");

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

	;
	if (is_dgemm) {
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
		legend.append("rect")
					.attr("x", function(d, i) { return 15; })
					.attr("y", function(d, i) { return 40 + i * 20; })
					.attr("width", 10)
					.attr("height", 10)
					.attr("class", function(d) { return "legend " + d.benchmark.toLowerCase(); })
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
