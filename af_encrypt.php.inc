<?php



trait af_encrypt {



	////////////////////////////////////////////////////////////////////////////
	// ENCRYPT A STRING, RETURNING AN ARRAY
	////////////////////////////////////////////////////////////////////////////
	public function encrypt($string) {
		$ivlen	= openssl_cipher_iv_length($this->config->pci['cipher']);
		$iv		= openssl_random_pseudo_bytes($ivlen);
		$hash	= hash_hmac('sha256', $string, $iv);

		return [
			'iv'	=> base64_encode($iv),
			'raw'	=> openssl_encrypt(
				$hash . ':' . $string,
				$this->config->pci['cipher'],
				$this->config->pci['key'] . $this->{'af.pci'},
				$options=0,
				$iv
			),
		];
	}




	////////////////////////////////////////////////////////////////////////////
	// DECRYPT A PREVIOUSLY ENCRYPTED STRING
	////////////////////////////////////////////////////////////////////////////
	public function decrypt($encrypted) {
		if (empty($encrypted['iv'])  ||  empty($encrypted['raw'])) return NULL;

		$iv = @base64_decode($encrypted['iv']);

		$decrypted = @openssl_decrypt(
			$encrypted['raw'],
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
