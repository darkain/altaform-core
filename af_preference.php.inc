<?php


//TODO: PLACE THIS IS DEDICATED AFUSER CLASS


trait af_preference {


	public function parseUserPreferences($string) {
		if (PHP_VERSION_ID >= 50400) {
			return json_decode($string, true, 512, JSON_BIGINT_AS_STRING);
		}

		return json_decode($string, true);
	}




	public function getUserPreferences($id=false) {
		global $user, $db;
		if (is_array($id)  &&  isset($id['user_id'])) $id = $id['user_id'];
		if (!is_numeric($id)) $id = $user['user_id'];
		$id = (int) $id;

		if (isset($this->_prefs[$id])) return $this->_prefs[$id];

		return $this->_prefs[$id] = self::parseUserPreferences(
			$db->cellId('pudl_user_preference', 'preference', 'user_id', $id)
		);
	}




	public function getUserPreference($path, $id=false, $default=false) {
		$prefs = $this->getUserPreferences($id);
		if (isset($prefs[ $path ])) return $prefs[ $path ];
		return $default;
	}




	public function setUserPreferences($preferences, $id=false) {
		global $user, $db;
		if (is_array($id)) $id = $id['user_id'];
		if (!is_numeric($id)) $id = $user['user_id'];
		$id = (int) $id;

		$this->_prefs[$id] = $preferences;

		return $db->replace('pudl_user_preference', [
			'user_id'		=> $id,
			'preference'	=> json_encode($preferences)
		], true);
	}




	public function updateUserPreference($key, $value, $id=false) {
		$prefs = self::getUserPreferences($id);
		$prefs[$key] = $value;
		return self::setUserPreferences($prefs, $id);
	}


}
