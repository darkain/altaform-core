<?php


////////////////////////////////////////////////////////////////////////////////
// RATIONAL TESTS (float to string)
////////////////////////////////////////////////////////////////////////////////

afUnit(
	afString::rational(1.5),
	'1-1/2'
);




afUnit(
	afString::rational(1),
	'1'
);




afUnit(
	afString::rational(1.25),
	'1-1/4'
);




afUnit(
	afString::rational(1.3333),
	'1-1/3'
);




afUnit(
	afString::rational(1.6666),
	'1-2/3'
);




afUnit(
	afString::rational(10.95),
	'10-19/20'
);




afUnit(
	afString::rational(0.5),
	'1/2'
);




////////////////////////////////////////////////////////////////////////////////
// UNRATIONAL TESTS (string to float)
////////////////////////////////////////////////////////////////////////////////

afUnit(
	afString::unrational('1-1/2'),
	1.5
);




afUnit(
	afString::unrational('1'),
	1.0
);




afUnit(
	afString::unrational('1-1/4'),
	1.25
);




afUnit(
	afString::unrational('1-1/3'),
	1.3333
);




afUnit(
	afString::unrational('1-2/3'),
	1.6667
);




afUnit(
	afString::unrational('10-19/20'),
	10.95
);




afUnit(
	afString::unrational('1/2'),
	0.5
);
