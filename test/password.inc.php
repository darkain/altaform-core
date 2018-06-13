<?php



//	TODO: ENABLE THIS SET OF UNIT TESTS OUTSIDE OF FULL ALTAFORM
if (!class_exists('afconfig')) return;




$afconfig('password', [
	'length'	=> 8,		//Minimum length
]);




afUnit(
	afUser::validatePassword('test'),
	'Password must be at least 8 characters long'
);




afUnit(
	afUser::validatePassword('test1324'),
	'Password must contain both upper and lower case letters'
);




afUnit(afUser::validatePassword('Test1324'));
