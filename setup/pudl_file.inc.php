<?php

/*


CREATE TABLE `pudl_file` (
  `file_hash` varbinary(128) NOT NULL COMMENT 'Hash value of file contents',
  `file_parent` varbinary(128) DEFAULT NULL,
  `file_size` bigint(20) UNSIGNED NOT NULL COMMENT 'File size on disc, in bytes',
  `file_uploaded` bigint(20) NOT NULL COMMENT 'UNIX timestamp of file upload time',
  `file_visible` set('admin','staff','debug','user','jobber','dealer','wholesale') COLLATE ascii_bin NOT NULL DEFAULT 'admin,staff,debug,user,jobber,dealer,wholesale',
  `mime_id` int(10) UNSIGNED DEFAULT NULL,
  `file_name` varchar(500) CHARACTER SET utf8mb4_general_ci NOT NULL,
  `file_width` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `file_height` smallint(5) UNSIGNED NOT NULL DEFAULT 0,
  `file_average` char(6) CHARACTER SET ascii DEFAULT NULL,
  `file_credits` int(10) unsigned NOT NULL DEFAULT 0,
  `file_comments` int(10) unsigned NOT NULL DEFAULT 0,
  `file_favorites` int(10) unsigned NOT NULL DEFAULT 0,
  `file_views` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`file_hash`),
  KEY `mime_id` (`mime_id`),
  KEY `file_name` (`file_name`),
  KEY `file_parent` (`file_parent`),
  KEY `file_credits` (`file_credits`),
  KEY `file_comments` (`file_comments`),
  KEY `file_favorites` (`file_favorites`),
  KEY `file_views` (`file_views`),
  CONSTRAINT `pudl_file_mime` FOREIGN KEY (`mime_id`) REFERENCES `pudl_mimetype` (`mime_id`) ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT `pudl_file_parent` FOREIGN KEY (`file_parent`) REFERENCES `pudl_file` (`file_hash`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;


CREATE TABLE `pudl_file_thumb` (
  `thumb_hash` varbinary(128) NOT NULL COMMENT 'Hash value of thumbnail contents',
  `file_hash` varbinary(128) NOT NULL,
  `thumb_size` bigint(20) UNSIGNED NOT NULL,
  `thumb_type` enum('50','100','150','200','500','640','800','1000','1280','1500','1920') CHARACTER SET ascii COLLATE ascii_bin NOT NULL,
  PRIMARY KEY (`thumb_hash`),
  KEY `file_hash` (`file_hash`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;


ALTER TABLE `pudl_mimetype`
  ADD CONSTRAINT `pudl_mime_icon` FOREIGN KEY (`mime_icon`) REFERENCES `pudl_file` (`file_hash`) ON DELETE SET NULL ON UPDATE CASCADE;
*/


$db(
	'CREATE TABLE ' . $db->identifiers('pudl_file_meta', true) . ' (
	  `file_hash` varbinary(128) NOT NULL,
	  `file_meta_name` varchar(32) COLLATE utf8mb4_bin NOT NULL,
	  `file_meta_value` varchar(255) COLLATE utf8mb4_bin NOT NULL,
	  PRIMARY KEY (`file_hash`,`file_meta_name`),
	  KEY `file_meta_name` (`file_meta_name`),
	  CONSTRAINT `pudl_file_meta` FOREIGN KEY (`file_hash`) REFERENCES ' . $db->identifiers('pudl_file', true) . ' (`file_hash`) ON DELETE CASCADE ON UPDATE CASCADE
	) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci'
);


/*

CREATE TABLE IF NOT EXISTS `pudl_file_user` (
  `file_hash` varbinary(128) NOT NULL,
  `user_id` bigint(20) UNSIGNED NOT NULL,
  `user_time` bigint(20) NOT NULL,
  `file_credit_by` bigint(20) UNSIGNED DEFAULT NULL,
  `file_user_visible` tinyint(1) NOT NULL DEFAULT '1',
  `file_text` text NOT NULL,
  PRIMARY KEY (`file_hash`,`user_id`),
  KEY `user_id` (`user_id`),
  KEY `file_credit_by` (`file_credit_by`),
  KEY `user_time` (`user_time`),
  CONSTRAINT `pudl_file_user_hash` FOREIGN KEY (`file_hash`) REFERENCES `pudl_file` (`file_hash`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `pudl_file_user_id` FOREIGN KEY (`user_id`) REFERENCES `pudl_user` (`user_id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `pudl_file_credit` FOREIGN KEY (`file_credit_by`) REFERENCES `pudl_user` (`user_id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

*/
