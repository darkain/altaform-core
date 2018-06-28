<?php



afUnit(
	afString::fromBytes(123),
	'123 B'
);




afUnit(
	afString::fromBytes(123 * AF_KILOBYTE),
	'123 KiB'
);




afUnit(
	afString::fromBytes(123 * AF_MEGABYTE),
	'123 MiB'
);




afUnit(
	afString::fromBytes(123 * AF_GIGABYTE),
	'123 GiB'
);




afUnit(
	afString::fromBytes(123, BYTE_FORMAT_KIB),
	'123 B'
);




afUnit(
	afString::fromBytes(123 * AF_KILOBYTE, BYTE_FORMAT_KIB),
	'123 KiB'
);




afUnit(
	afString::fromBytes(123 * AF_MEGABYTE, BYTE_FORMAT_KIB),
	'123 MiB'
);




afUnit(
	afString::fromBytes(123 * AF_GIGABYTE, BYTE_FORMAT_KIB),
	'123 GiB'
);




afUnit(
	afString::fromBytes(123, BYTE_FORMAT_KIB, '-'),
	'123-B'
);




afUnit(
	afString::fromBytes(123 * AF_KILOBYTE, BYTE_FORMAT_KIB, '-'),
	'123-KiB'
);




afUnit(
	afString::fromBytes(123 * AF_MEGABYTE, BYTE_FORMAT_KIB, '-'),
	'123-MiB'
);




afUnit(
	afString::fromBytes(123 * AF_GIGABYTE, BYTE_FORMAT_KIB, '-'),
	'123-GiB'
);




afUnit(
	afString::fromBytes(123, BYTE_FORMAT_KB),
	'123 B'
);




afUnit(
	afString::fromBytes(123 * AF_KILOBYTE, BYTE_FORMAT_KB),
	'126 KB'
);




afUnit(
	afString::fromBytes(123 * AF_MEGABYTE, BYTE_FORMAT_KB),
	'129 MB'
);




afUnit(
	afString::fromBytes(123 * AF_GIGABYTE, BYTE_FORMAT_KB),
	'132.1 GB'
);




afUnit(
	afString::fromBytes(123, BYTE_FORMAT_K),
	'123 B'
);




afUnit(
	afString::fromBytes(123 * AF_KILOBYTE, BYTE_FORMAT_K),
	'123 K'
);




afUnit(
	afString::fromBytes(123 * AF_MEGABYTE, BYTE_FORMAT_K),
	'123 M'
);




afUnit(
	afString::fromBytes(123 * AF_GIGABYTE, BYTE_FORMAT_K),
	'123 G'
);




afUnit(
	afString::toBytes('123'),
	123
);




afUnit(
	afString::toBytes('123K'),
	123 * AF_KILOBYTE
);




afUnit(
	afString::toBytes('123M'),
	123 * AF_MEGABYTE
);




afUnit(
	afString::toBytes('123G'),
	123 * AF_GIGABYTE
);



afUnit(
	afString::toBytes('123 KB'),
	123*1000
);




afUnit(
	afString::toBytes('123 MB'),
	123*1000*1000
);




afUnit(
	afString::toBytes('123 GB'),
	123*1000*1000*1000
);




afUnit(
	afString::toBytes('123 KiB'),
	123 * AF_KILOBYTE
);




afUnit(
	afString::toBytes('123 MiB'),
	123 * AF_MEGABYTE
);




afUnit(
	afString::toBytes('123 GiB'),
	123 * AF_GIGABYTE
);




afUnit(
	afString::toBytes('123K'),
	123 * AF_KILOBYTE
);




afUnit(
	afString::toBytes('123M'),
	123 * AF_MEGABYTE
);




afUnit(
	afString::toBytes('123G'),
	123 * AF_GIGABYTE
);




afUnit(
	afString::toBytes('123-K'),
	123 * AF_KILOBYTE
);




afUnit(
	afString::toBytes('123-M'),
	123 * AF_MEGABYTE
);




afUnit(
	afString::toBytes('123-G'),
	123 * AF_GIGABYTE
);
