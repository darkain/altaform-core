<?php


trait af_preference {


	public static function parsePreferences($data) {
		if (empty($data)) return [];

		if (PHP_VERSION_ID >= 50400) {
			return json_decode($data, true, 512, JSON_BIGINT_AS_STRING);
		}

		return json_decode($data, true);
	}




	public function getPreferences() {
		global $db;

		if (!empty($this->_prefs)) return $this->_prefs;

		if (!empty($this->preference)) {
			return $this->_prefs = $this->parsePreferences($this->preference);
		}

		return $this->_prefs = $this->parsePreferences(
			$db->cellId('pudl_user_preference', 'preference', 'user_id', $this)
		);
	}




	public function getPreference($name, $default=false) {
		$prefs = $this->getPreferences();
		return isset($prefs[$name]) ? $prefs[$name] : $default;
	}




	public function hasPreference($name) {
		$prefs = $this->getPreferences();
		return isset($prefs[$name]);
	}




	public function setPreferences($preferences) {
		global $db;

		$this->_prefs		= $preferences;
		$this->preference	= json_encode($preferences);

		return $db->replace('pudl_user_preference', [
			'user_id'		=> $this,
			'preference'	=> $this->preference,
		], true);
	}




	public function updatePreference($key, $value) {
		$prefs = $this->getPreferences();
		$prefs[$key] = $value;
		return $this->setPreferences($prefs);
	}



	private				$_prefs		= [];

}
