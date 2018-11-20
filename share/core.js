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
