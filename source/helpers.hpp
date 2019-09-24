char CreateByteWithNibbles(int hi, int lo) {
	char byte = 0;
	byte = (byte & 0xF0) | (lo & 0xF); // write low nibble
	byte = (byte & 0x0F) | ((hi & 0xF) << 4); // write high nibble
}

char SetBit(char charToModify, int index, bool value) {
	charToModify ^= (-(unsigned long) value ^ charToModify) & (1UL << index);
	return charToModify;
}