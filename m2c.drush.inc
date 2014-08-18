<?php

/**
 * @file
 * Drush make to Composer (m2c)
 */

/**
 * Implements hook_drush_help().
 */
function m2c_drush_help($section) {
  switch($section) {
    case 'drush:m2c':
      return dt("Drush make to Composer file conversion");
    break;
  }
}

/**
 * Implements hook_drush_command().
 */
function m2c_drush_command() {
  $items = array();
  $items['m2c'] = array(
    'description' => 'Convert a Drush makefile to composer format, on standard output',
    'arguments' => array(
      'make' => 'Makefile location',
    ),
    'options' => array(
      // 'name' => 'description',
    ),
    'aliases' => array('make2composer'),

    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );

  return $items;
}

/**
 * Implements hook_command_validate().
 *
 * Check makefile location provided, and that it exists.
 */
function drush_m2c_validate($make = NULL) {
  if (!$make) {
    drush_set_error("DRUSH_M2C_NO_MAKEFILE", "You must specify a makefile.");
    return;
  }
  if (!file_exists($make)) {
    drush_set_error("DRUSH_M2C_NO_MAKEFILE", "Makefile does not exist: '$make'.");
    return;
  }
}

/**
 * Drush command hook.
 */
function drush_m2c($make = NULL) {
  drush_log("Command hook TODO.", "error");
}
