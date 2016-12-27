<?php

require_once('geolocation.php.inc');



class afaudit {


	static function insert($object, $type, $value_new, $value_old=NULL) {
		global $db, $user;

		$db->insert('pudl_audit', [
			'user_id'			=> $user->id(),
			'user_ip'			=> afgeo::ipaddress(),
			'object_id'			=> (int) $object,
			'object_type_id'	=> altaform::type($type),
			'audit_old'			=> $value_old,
			'audit_new'			=> $value_new,
			'audit_timestamp'	=> $db->time(),
		]);
	}




	static function action($object, $type, $action, $value_new=NULL, $value_old=NULL) {
		global $db, $user;

		$db->insert('pudl_audit', [
			'user_id'			=> $user->id(),
			'user_ip'			=> afgeo::ipaddress(),
			'object_id'			=> (int) $object,
			'object_type_id'	=> altaform::type($type),
			'audit_old'			=> $value_old,
			'audit_new'			=> $value_new,
			'audit_action'		=> $action,
			'audit_timestamp'	=> $db->time(),
		]);
	}


}
