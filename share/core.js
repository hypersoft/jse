String.prototype.chomp = function chomp() {
	return this.replace(/[\s\uFEFF\xA0]+$/g, '');
}

run.prototype = {
	toString:function(){return this.stdout || this.stderr || this.status},
	valueOf:function(){return this.status}
};

Object.defineProperty(Number.prototype, {
	toHex:{value:function hex(number) {
		return '0x' + Math.abs(number).toString(16);
	}}
});