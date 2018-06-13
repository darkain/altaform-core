<?php




afUnit(
	afString::ltrim('test'),
	'test'
);




afUnit(
	afString::ltrim(' test '),
	'test '
);




afUnit(
	afString::rtrim('test'),
	'test'
);




afUnit(
	afString::rtrim(' test '),
	' test'
);




afUnit(
	afString::trim('test'),
	'test'
);




afUnit(
	afString::trim(' test'),
	'test'
);




afUnit(
	afString::trim('test '),
	'test'
);




afUnit(
	afString::trim(' test '),
	'test'
);




afUnit(
	afString::doublespace('test'),
	'test'
);




afUnit(
	afString::doublespace('te st'),
	'te st'
);




afUnit(
	afString::doublespace('te  st'),
	'te st'
);




afUnit(
	afString::doublespace("t \t e \r s \n t"),
	't e s t'
);




afUnit(
	afString::doublespace("  \r  te  \n  st  \0  "),
	' te st '
);




afUnit(
	afString::doublespace("a \x00 b \x09 c \x0b d \x10 e \x13 f \x20 g"),
	'a b c d e f g'
);
