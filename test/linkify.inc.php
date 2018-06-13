<?php

afUnit(
	afString::linkify('test'),
	'test'
);




afUnit(
	afString::linkify(' test'),
	' test'
);




afUnit(
	afString::linkify('test '),
	'test '
);




afUnit(
	afString::linkify(' test '),
	' test '
);




afUnit(
	afString::linkify('-test-'),
	'-test-'
);




afUnit(
	afString::linkify('- test -'),
	'- test -'
);




afUnit(
	afString::linkify('--test--'),
	'--test--'
);







afUnit(
	afString::linkify('http://example.com'),
	'<a href="http://example.com" target="_blank">http://example.com</a>'
);




afUnit(
	afString::linkify('http://example.com/'),
	'<a href="http://example.com/" target="_blank">http://example.com/</a>'
);




afUnit(
	afString::linkify('http://www.example.com'),
	'<a href="http://www.example.com" target="_blank">http://www.example.com</a>'
);




afUnit(
	afString::linkify('http://www.example.com/'),
	'<a href="http://www.example.com/" target="_blank">http://www.example.com/</a>'
);




afUnit(
	afString::linkify('https://example.com'),
	'<a href="https://example.com" target="_blank">https://example.com</a>'
);




afUnit(
	afString::linkify('https://example.com/'),
	'<a href="https://example.com/" target="_blank">https://example.com/</a>'
);




afUnit(
	afString::linkify('https://www.example.com'),
	'<a href="https://www.example.com" target="_blank">https://www.example.com</a>'
);




afUnit(
	afString::linkify('https://www.example.com/'),
	'<a href="https://www.example.com/" target="_blank">https://www.example.com/</a>'
);




afUnit(
	afString::linkify('ftp://example.com'),
	'<a href="ftp://example.com" target="_blank">ftp://example.com</a>'
);




afUnit(
	afString::linkify('ftp://example.com/'),
	'<a href="ftp://example.com/" target="_blank">ftp://example.com/</a>'
);




afUnit(
	afString::linkify('ftp://www.example.com'),
	'<a href="ftp://www.example.com" target="_blank">ftp://www.example.com</a>'
);




afUnit(
	afString::linkify('ftp://www.example.com/'),
	'<a href="ftp://www.example.com/" target="_blank">ftp://www.example.com/</a>'
);



afUnit(
	afString::linkify('ftps://example.com'),
	'<a href="ftps://example.com" target="_blank">ftps://example.com</a>'
);




afUnit(
	afString::linkify('ftps://example.com/'),
	'<a href="ftps://example.com/" target="_blank">ftps://example.com/</a>'
);




afUnit(
	afString::linkify('ftps://www.example.com'),
	'<a href="ftps://www.example.com" target="_blank">ftps://www.example.com</a>'
);




afUnit(
	afString::linkify('ftps://www.example.com/'),
	'<a href="ftps://www.example.com/" target="_blank">ftps://www.example.com/</a>'
);




afUnit(
	afString::linkify('unknown://example.com'),
	'unknown://example.com'
);




afUnit(
	afString::linkify('unknown://example.com/'),
	'unknown://example.com/'
);




afUnit(
	afString::linkify('unknown://www.example.com'),
	'unknown://www.example.com'
);




afUnit(
	afString::linkify('unknown://www.example.com/'),
	'unknown://www.example.com/'
);




afUnit(	//TODO: FIX THIS CASE
	afString::linkify('example.com'),
	'example.com'
);




afUnit(	//TODO: FIX THIS CASE
	afString::linkify('example.com/'),
	'example.com/'
);




afUnit(
	afString::linkify('www.example.com'),
	'<a href="www.example.com" target="_blank">www.example.com</a>'
);




afUnit(
	afString::linkify('www.example.com/'),
	'<a href="www.example.com/" target="_blank">www.example.com/</a>'
);






afUnit(
	afString::linkify('<a href="http://example.com">TEST</a>'),
	'&lt;a href="<a href="http://example.com" target="_blank">http://example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="http://example.com/">TEST</a>'),
	'&lt;a href="<a href="http://example.com/" target="_blank">http://example.com/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="http://www.example.com">TEST</a>'),
	'&lt;a href="<a href="http://www.example.com" target="_blank">http://www.example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="http://www.example.com/">TEST</a>'),
	'&lt;a href="<a href="http://www.example.com/" target="_blank">http://www.example.com/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="https://example.com">TEST</a>'),
	'&lt;a href="<a href="https://example.com" target="_blank">https://example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="https://example.com/">TEST</a>'),
	'&lt;a href="<a href="https://example.com/" target="_blank">https://example.com/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="https://www.example.com">TEST</a>'),
	'&lt;a href="<a href="https://www.example.com" target="_blank">https://www.example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="https://www.example.com/">TEST</a>'),
	'&lt;a href="<a href="https://www.example.com/" target="_blank">https://www.example.com/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="ftp://example.com">TEST</a>'),
	'&lt;a href="<a href="ftp://example.com" target="_blank">ftp://example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="ftp://example.com/">TEST</a>'),
	'&lt;a href="<a href="ftp://example.com/" target="_blank">ftp://example.com/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="ftp://www.example.com">TEST</a>'),
	'&lt;a href="<a href="ftp://www.example.com" target="_blank">ftp://www.example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="ftp://www.example.com/">TEST</a>'),
	'&lt;a href="<a href="ftp://www.example.com/" target="_blank">ftp://www.example.com/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="unknown://example.com">TEST</a>'),
	'&lt;a href="unknown://example.com"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="unknown://example.com/">TEST</a>'),
	'&lt;a href="unknown://example.com/"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="unknown://www.example.com">TEST</a>'),
	'&lt;a href="unknown://www.example.com"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="unknown://www.example.com/">TEST</a>'),
	'&lt;a href="unknown://www.example.com/"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="example.com">TEST</a>'),
	'&lt;a href="example.com"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="example.com/">TEST</a>'),
	'&lt;a href="example.com/"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="www.example.com">TEST</a>'),
	'&lt;a href="<a href="www.example.com" target="_blank">www.example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="www.example.com/">TEST</a>'),
	'&lt;a href="<a href="www.example.com/" target="_blank">www.example.com/</a>"&gt;TEST&lt;/a&gt;'
);






afUnit(
	afString::linkify("<a href='http://example.com'>TEST</a>"),
	'&lt;a href=\'<a href="http://example.com" target="_blank">http://example.com</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='http://example.com/'>TEST</a>"),
	'&lt;a href=\'<a href="http://example.com/" target="_blank">http://example.com/</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='http://www.example.com'>TEST</a>"),
	'&lt;a href=\'<a href="http://www.example.com" target="_blank">http://www.example.com</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='http://www.example.com/'>TEST</a>"),
	'&lt;a href=\'<a href="http://www.example.com/" target="_blank">http://www.example.com/</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='https://example.com'>TEST</a>"),
	'&lt;a href=\'<a href="https://example.com" target="_blank">https://example.com</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='https://example.com/'>TEST</a>"),
	'&lt;a href=\'<a href="https://example.com/" target="_blank">https://example.com/</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='https://www.example.com'>TEST</a>"),
	'&lt;a href=\'<a href="https://www.example.com" target="_blank">https://www.example.com</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='https://www.example.com/'>TEST</a>"),
	'&lt;a href=\'<a href="https://www.example.com/" target="_blank">https://www.example.com/</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='ftp://example.com'>TEST</a>"),
	'&lt;a href=\'<a href="ftp://example.com" target="_blank">ftp://example.com</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='ftp://example.com/'>TEST</a>"),
	'&lt;a href=\'<a href="ftp://example.com/" target="_blank">ftp://example.com/</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='ftp://www.example.com'>TEST</a>"),
	'&lt;a href=\'<a href="ftp://www.example.com" target="_blank">ftp://www.example.com</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='ftp://www.example.com/'>TEST</a>"),
	'&lt;a href=\'<a href="ftp://www.example.com/" target="_blank">ftp://www.example.com/</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='unknown://example.com'>TEST</a>"),
	'&lt;a href=\'unknown://example.com\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='unknown://example.com/'>TEST</a>"),
	'&lt;a href=\'unknown://example.com/\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='unknown://www.example.com'>TEST</a>"),
	'&lt;a href=\'unknown://www.example.com\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='unknown://www.example.com/'>TEST</a>"),
	'&lt;a href=\'unknown://www.example.com/\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='example.com'>TEST</a>"),
	'&lt;a href=\'example.com\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='example.com/'>TEST</a>"),
	'&lt;a href=\'example.com/\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='www.example.com'>TEST</a>"),
	'&lt;a href=\'<a href="www.example.com" target="_blank">www.example.com</a>\'&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify("<a href='www.example.com/'>TEST</a>"),
	'&lt;a href=\'<a href="www.example.com/" target="_blank">www.example.com/</a>\'&gt;TEST&lt;/a&gt;'
);






afUnit(
	afString::linkify('<a href="http://m.example.com">TEST</a>'),
	'&lt;a href="<a href="http://m.example.com" target="_blank">http://m.example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="http://m.example.com/">TEST</a>'),
	'&lt;a href="<a href="http://m.example.com/" target="_blank">http://m.example.com/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="https://m.example.com">TEST</a>'),
	'&lt;a href="<a href="https://m.example.com" target="_blank">https://m.example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="https://m.example.com/">TEST</a>'),
	'&lt;a href="<a href="https://m.example.com/" target="_blank">https://m.example.com/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="ftp://m.example.com">TEST</a>'),
	'&lt;a href="<a href="ftp://m.example.com" target="_blank">ftp://m.example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="ftp://m.example.com/">TEST</a>'),
	'&lt;a href="<a href="ftp://m.example.com/" target="_blank">ftp://m.example.com/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="unknown://m.example.com">TEST</a>'),
	'&lt;a href="unknown://m.example.com"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="unknown://m.example.com/">TEST</a>'),
	'&lt;a href="unknown://m.example.com/"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="m.example.com">TEST</a>'),
	'&lt;a href="<a href="m.example.com" target="_blank">m.example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="m.example.com/">TEST</a>'),
	'&lt;a href="<a href="m.example.com/" target="_blank">m.example.com/</a>"&gt;TEST&lt;/a&gt;'
);






afUnit(
	afString::linkify('<a href="http://x.example.com">TEST</a>'),
	'&lt;a href="<a href="http://x.example.com" target="_blank">http://x.example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="http://x.example.com/">TEST</a>'),
	'&lt;a href="<a href="http://x.example.com/" target="_blank">http://x.example.com/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="https://x.example.com">TEST</a>'),
	'&lt;a href="<a href="https://x.example.com" target="_blank">https://x.example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="https://x.example.com/">TEST</a>'),
	'&lt;a href="<a href="https://x.example.com/" target="_blank">https://x.example.com/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="ftp://x.example.com">TEST</a>'),
	'&lt;a href="<a href="ftp://x.example.com" target="_blank">ftp://x.example.com</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="ftp://x.example.com/">TEST</a>'),
	'&lt;a href="<a href="ftp://x.example.com/" target="_blank">ftp://x.example.com/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="unknown://x.example.com">TEST</a>'),
	'&lt;a href="unknown://x.example.com"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="unknown://x.example.com/">TEST</a>'),
	'&lt;a href="unknown://x.example.com/"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="x.example.com">TEST</a>'),
	'&lt;a href="x.example.com"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="x.example.com/">TEST</a>'),
	'&lt;a href="x.example.com/"&gt;TEST&lt;/a&gt;'
);






afUnit(
	afString::linkify('http://203.0.113.1'),
	'<a href="http://203.0.113.1" target="_blank">http://203.0.113.1</a>'
);




afUnit(
	afString::linkify('http://203.0.113.1/'),
	'<a href="http://203.0.113.1/" target="_blank">http://203.0.113.1/</a>'
);




afUnit(
	afString::linkify('https://203.0.113.1'),
	'<a href="https://203.0.113.1" target="_blank">https://203.0.113.1</a>'
);




afUnit(
	afString::linkify('https://203.0.113.1/'),
	'<a href="https://203.0.113.1/" target="_blank">https://203.0.113.1/</a>'
);




afUnit(
	afString::linkify('ftp://203.0.113.1'),
	'<a href="ftp://203.0.113.1" target="_blank">ftp://203.0.113.1</a>'
);




afUnit(
	afString::linkify('ftp://203.0.113.1/'),
	'<a href="ftp://203.0.113.1/" target="_blank">ftp://203.0.113.1/</a>'
);




afUnit(	//TODO: THIS IS INCONSISTENT WITH THE ITEM BELOW
	afString::linkify('unknown://203.0.113.1'),
	'unknown://203.0.113.1'
);




afUnit(
	afString::linkify('unknown://203.0.113.1/'),
	'unknown://<a href="203.0.113.1/" target="_blank">203.0.113.1/</a>'
);




afUnit(	//TODO: IP ADDRESS SHOULD ASSUME HTTP://
	afString::linkify('203.0.113.1'),
	'203.0.113.1'
);




afUnit(
	afString::linkify('203.0.113.1/'),
	'<a href="203.0.113.1/" target="_blank">203.0.113.1/</a>'
);





afUnit(
	afString::linkify('http://203.0.113'),
	'<a href="http://203.0.113" target="_blank">http://203.0.113</a>'
);




afUnit(
	afString::linkify('http://203.0.113/'),
	'<a href="http://203.0.113/" target="_blank">http://203.0.113/</a>'
);




afUnit(
	afString::linkify('https://203.0.113'),
	'<a href="https://203.0.113" target="_blank">https://203.0.113</a>'
);




afUnit(
	afString::linkify('https://203.0.113/'),
	'<a href="https://203.0.113/" target="_blank">https://203.0.113/</a>'
);




afUnit(
	afString::linkify('ftp://203.0.113'),
	'<a href="ftp://203.0.113" target="_blank">ftp://203.0.113</a>'
);




afUnit(
	afString::linkify('ftp://203.0.113/'),
	'<a href="ftp://203.0.113/" target="_blank">ftp://203.0.113/</a>'
);




afUnit(
	afString::linkify('unknown://203.0.113'),
	'unknown://203.0.113'
);




afUnit(
	afString::linkify('unknown://203.0.113/'),
	'unknown://203.0.113/'
);




afUnit(
	afString::linkify('203.0.113'),
	'203.0.113'
);




afUnit(
	afString::linkify('203.0.113/'),
	'203.0.113/'
);





afUnit(
	afString::linkify('http://203.0'),
	'<a href="http://203.0" target="_blank">http://203.0</a>'
);




afUnit(
	afString::linkify('http://203.0/'),
	'<a href="http://203.0/" target="_blank">http://203.0/</a>'
);




afUnit(
	afString::linkify('https://203.0'),
	'<a href="https://203.0" target="_blank">https://203.0</a>'
);




afUnit(
	afString::linkify('https://203.0/'),
	'<a href="https://203.0/" target="_blank">https://203.0/</a>'
);




afUnit(
	afString::linkify('ftp://203.0'),
	'<a href="ftp://203.0" target="_blank">ftp://203.0</a>'
);




afUnit(
	afString::linkify('ftp://203.0/'),
	'<a href="ftp://203.0/" target="_blank">ftp://203.0/</a>'
);




afUnit(
	afString::linkify('unknown://203.0'),
	'unknown://203.0'
);




afUnit(
	afString::linkify('unknown://203.0/'),
	'unknown://203.0/'
);




afUnit(
	afString::linkify('203.0'),
	'203.0'
);




afUnit(
	afString::linkify('203.0/'),
	'203.0/'
);





afUnit(
	afString::linkify('http://203'),
	'<a href="http://203" target="_blank">http://203</a>'
);




afUnit(
	afString::linkify('http://203/'),
	'<a href="http://203/" target="_blank">http://203/</a>'
);




afUnit(
	afString::linkify('https://203'),
	'<a href="https://203" target="_blank">https://203</a>'
);




afUnit(
	afString::linkify('https://203/'),
	'<a href="https://203/" target="_blank">https://203/</a>'
);




afUnit(
	afString::linkify('ftp://203'),
	'<a href="ftp://203" target="_blank">ftp://203</a>'
);




afUnit(
	afString::linkify('ftp://203/'),
	'<a href="ftp://203/" target="_blank">ftp://203/</a>'
);




afUnit(
	afString::linkify('unknown://203'),
	'unknown://203'
);




afUnit(
	afString::linkify('unknown://203/'),
	'unknown://203/'
);




afUnit(
	afString::linkify('203'),
	'203'
);




afUnit(
	afString::linkify('203/'),
	'203/'
);




afUnit(
	afString::linkify('<a href="http://203.0.113.1">TEST</a>'),
	'&lt;a href="<a href="http://203.0.113.1" target="_blank">http://203.0.113.1</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="http://203.0.113.1/">TEST</a>'),
	'&lt;a href="<a href="http://203.0.113.1/" target="_blank">http://203.0.113.1/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="https://203.0.113.1">TEST</a>'),
	'&lt;a href="<a href="https://203.0.113.1" target="_blank">https://203.0.113.1</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="https://203.0.113.1/">TEST</a>'),
	'&lt;a href="<a href="https://203.0.113.1/" target="_blank">https://203.0.113.1/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="ftp://203.0.113.1">TEST</a>'),
	'&lt;a href="<a href="ftp://203.0.113.1" target="_blank">ftp://203.0.113.1</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="ftp://203.0.113.1/">TEST</a>'),
	'&lt;a href="<a href="ftp://203.0.113.1/" target="_blank">ftp://203.0.113.1/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="unknown://203.0.113.1">TEST</a>'),
	'&lt;a href="unknown://203.0.113.1"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="unknown://203.0.113.1/">TEST</a>'),
	'&lt;a href="unknown://<a href="203.0.113.1/" target="_blank">203.0.113.1/</a>"&gt;TEST&lt;/a&gt;'
);




afUnit(	//TODO: IP ADDRESS NOT DETECTING WITHOUT TRAILING /
	afString::linkify('<a href="203.0.113.1">TEST</a>'),
	'&lt;a href="203.0.113.1"&gt;TEST&lt;/a&gt;'
);




afUnit(
	afString::linkify('<a href="203.0.113.1/">TEST</a>'),
	'&lt;a href="<a href="203.0.113.1/" target="_blank">203.0.113.1/</a>"&gt;TEST&lt;/a&gt;'
);





afUnit(	//TODO: IPV6 SUPPORT
	afString::linkify('[2001:DB8:BAD:C0DE::1]'),
	'[2001:DB8:BAD:C0DE::1]'
);




afUnit(	//TODO: IPV6 SUPPORT
	afString::linkify('http://[2001:DB8:BAD:C0DE::1]'),
	'http://[2001:DB8:BAD:C0DE::1]'
);




afUnit(	//TODO: IPV6 SUPPORT
	afString::linkify('https://[2001:DB8:BAD:C0DE::1]'),
	'https://[2001:DB8:BAD:C0DE::1]'
);




afUnit(	//TODO: IPV6 SUPPORT
	afString::linkify('ftp://[2001:DB8:BAD:C0DE::1]'),
	'ftp://[2001:DB8:BAD:C0DE::1]'
);




afUnit(	//TODO: IPV6 SUPPORT
	afString::linkify('unknown://[2001:DB8:BAD:C0DE::1]'),
	'unknown://[2001:DB8:BAD:C0DE::1]'
);




