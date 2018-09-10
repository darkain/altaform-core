<?php

/*
CREATE TABLE `h12_user` (
  `user_id` bigint(20) UNSIGNED NOT NULL,
  `user_alias` varchar(100) CHARACTER SET ascii COLLATE ascii_bin DEFAULT NULL,
  `user_permission` set('admin','staff','debug','user','guest','banned') CHARACTER SET ascii COLLATE ascii_bin NOT NULL DEFAULT 'user',
  `user_json` varchar(4096) CHARACTER SET utf8mb4 DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE `h12_user_auth` (
  `user_id` bigint(20) UNSIGNED NOT NULL,
  `auth_account` varchar(100) CHARACTER SET utf8 NOT NULL,
  `auth_password` varchar(255) CHARACTER SET utf8 NOT NULL,
  `auth_verified` bigint(20) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
*/
