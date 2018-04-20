<?php

require_once('geolocation.php.inc');



class af_audit {


	static function insert($object, $type, $value_new, $value_old=NULL) {
		global $db, $user;

		if (!is_int($type)  &&  !ctype_digit($type)) {
			$type = altaform::type((int)$type);
		}

		$db->insert('pudl_audit', [
			'user_id'			=> $user->id(),
			'user_ip'			=> afgeo::ipaddress(),
			'object_id'			=> (int) $object,
			'object_type_id'	=> $type,
			'audit_old'			=> $value_old,
			'audit_new'			=> $value_new,
			'audit_timestamp'	=> $db->time(),
		]);
	}




	static function action($object, $type, $action, $value_new=NULL, $value_old=NULL) {
		global $db, $user;

		if (!is_int($type)  &&  !ctype_digit($type)) {
			$type = altaform::type($type);
		}

		$db->insert('pudl_audit', [
			'user_id'			=> $user->id(),
			'user_ip'			=> afgeo::ipaddress(),
			'object_id'			=> (int) $object,
			'object_type_id'	=> (int) $type,
			'audit_old'			=> $value_old,
			'audit_new'			=> $value_new,
			'audit_action'		=> $action,
			'audit_timestamp'	=> $db->time(),
		]);
	}


}
