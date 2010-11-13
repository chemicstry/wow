CREATE TABLE `gm_events` (
  `eventid` int(11) NOT NULL,
  `active` varchar(255) NOT NULL default '0',
  `eventx` varchar(255) default NULL,
  `eventy` varchar(255) default NULL,
  `eventz` varchar(255) default NULL,
  `eventmap` varchar(255) default NULL,
  PRIMARY KEY  (`eventid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;