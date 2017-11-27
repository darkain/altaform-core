<?php



trait af_encrypt {



	////////////////////////////////////////////////////////////////////////////
	// ENCRYPT A STRING, RETURNING A JSON STRING READY FOR DATABASE INSERT
	////////////////////////////////////////////////////////////////////////////
	public function encrypt($string) {
		$ivlen	= openssl_cipher_iv_length($this->config->pci['cipher']);
		$iv		= openssl_random_pseudo_bytes($ivlen);
		$hash	= hash_hmac('sha256', $string, $iv);

		return json_encode([
			'iv'	=> base64_encode($iv),
			'raw'	=> openssl_encrypt(
				$hash . ':' . $string,
				$this->config->pci['cipher'],
				$this->config->pci['key'] . $this->{'af.pci'},
				$options=0,
				$iv
			),
		]);
	}




	////////////////////////////////////////////////////////////////////////////
	// DECRYPT A PREVIOUSLY JSON ENCODED ENCRYPTED STRING
	////////////////////////////////////////////////////////////////////////////
	public function decrypt($json) {
		if (is_string($json)) $json = json_decode($json);
		if (is_object($json)) $json = (array) $json;

		if (empty($json['iv'])  ||  empty($json['raw'])) return NULL;

		$iv = @base64_decode($json['iv']);

		$decrypted = @openssl_decrypt(
			$json['raw'],
			$this->config->pci['cipher'],
			$this->config->pci['key'] . $this->{'af.pci'},
			$options=0,
			$iv
		);

		$pos	= strpos($decrypted, ':');
		if (empty($pos)) return NULL;

		$string	= substr($decrypted, $pos+1);
		$hash	= substr($decrypted, 0, $pos);

		return ($hash === hash_hmac('sha256', $string, $iv))
				? $string
				: NULL;
	}


}
