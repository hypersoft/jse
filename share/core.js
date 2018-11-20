String.prototype.chomp = function chomp() {
	return this.replace(/[\s\uFEFF\xA0]+$/g, '');
}

run.prototype = {
	toString:function(){return this.stdout || this.stderr || this.status},
	valueOf:function(){return this.status}
};

Object.defineProperties(Number.prototype, {
	toHex:{value:function hex(number) {
		// you can parse this result with Number(VAL)
		return '0x' + Math.abs(number).toString(16);
	}},
	toOctal:{value:function hex(number) {
		// you can parse this result with parseInt(VAL, 8)
		return Math.abs(number).toString(8);
	}},
	toBinary:{value:function hex(number) {
		// you can parse this result with parseInt(VAL, 2)
		return Math.abs(number).toString(2);
	}},
});

var File = function File(path) {
	if (this.constructor !== File) return new File(path);
	this.___path___ = path;
}

Object.defineProperties(File, {
	separator:{value: '/'},
	pathSeparator:{value: ':'},
	specificationSeparator:{value:'.'},
	// readText:{},
	// writeText:{},
	// appendText:{},
	// truncate:{},
	// writeData:{},
	// readData:{},
	// map:{},
	// open:{},
	// close:{},
	prototype:{value:Object.defineProperties({}, {
		___path___:{value:null, enumerable:false},
		toString:{value:function(){return ___path___;}},
		path:{get(){return ___path___;}},
		constructor:{value:File},
		getChildren:{value:function(){}},
		hasLegalName:{value:function(){/* validate name against shell operation corruption */}},
		isEmpty:{value:function(){}},
		isDirectory:{value:function(){}},
		isNewerThan:{value:function(){}},
		isOlderThan:{value:function(){}},
		isReadable:{value:function(){}},
		isWritable:{value:function(){}},
		isLink:{value:function(){}},
		isTeletype:{value:function(){/* is this pseudo file on a TTY? */}},
		isReal:{value:function(){/* is this a real file or a pseudo file? */}},
		isExecutable:{value:function(){}},
		isRelative:{value:function(){return ! this.path.startsWith(File.separator);}},
		realPath:{get(){}},
		parent:{get(){
			if (this.path === '/') return undefined;
			var parent = this.path.slice(0, this.path.lastIndexOf(File.separator));
			return new File(parent);
		}},
		specifications:{get(){
			var firstDotPosition = this.path.indexOf(File.specificationSeparator);
			if (! firstDotPosition ) return [];
			return this.path.slice(firstDotPosition).split(File.specificationSeparator);
		}},
		exists:{get(){}},
		creationTime:{get(){}},
		lastOperationTime:{get(){}},
		modificationTime:{get(){}},
		owner:{get(){}},
		group:{get(){}},
		size:{get(){}},
		storageSize:{get(){}},
		storageAvailable:{get(){}},
	})},
});
