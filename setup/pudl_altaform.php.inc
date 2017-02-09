<?php


$db->create('pudl_altaform', [
	'af_key'	=> 'varchar(255) NOT NULL',
	'af_value'	=> 'varchar(21588) NOT NULL',
], false, 'ENGINE=InnoDB DEFAULT CHARSET=utf8');


$db(
	'ALTER TABLE ' . $db->_table('pudl_altaform') .
	'	ADD PRIMARY KEY (' . $db->identifier('af_key') . '),' .
	'	ADD KEY ' . $db->identifier('af_value') .
		' (' . $db->identifier('af_value') . '(255))'
);
