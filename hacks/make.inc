<?php

/**
 * @file
 * Stubbed-out functions from Drush 6 make.
 */

/**
 * Hacked version of make_projects().
 */
function _make_projects_hacked($recursion, $contrib_destination, $info, $build_path, $make_dir) {
  drush_include_engine('release_info', 'updatexml');

  $projects = array();
  if (empty($info['projects'])) {
    if (drush_get_option('no-core') || $recursion) {
      return TRUE;
    }
    else {
      drush_set_error('MAKE_NO_CORE', dt('No core project specified.'));
      return FALSE;
    }
  }
  $ignore_checksums = drush_get_option('ignore-checksums');
  $translations = array();
  if (isset($info['translations'])) {
    $translations = $info['translations'];
  }
  if ($arg_translations = drush_get_option('translations', FALSE)) {
    $translations = array_merge(explode(',', $arg_translations), $translations);
  }
  foreach ($info['projects'] as $key => $project) {
    $md5 = '';
    if (isset($project['md5'])) {
      $md5 = $project['md5'];
    }
    // HACK: Drush make format used to permit version number as string.
    // This persists in some old makefiles, so cope with it.
    if (!is_array($project) && ($project * 1)) {
      $project = array("version" => $project);
    }
    // ENDHACK
    // Merge the known data onto the project info.
    $project += array(
      'name'                => $key,
      'core'                => $info['core'],
      'translations'        => $translations,
      'build_path'          => $build_path,
      'contrib_destination' => $contrib_destination,
      'version'             => '',
      'location'            => drush_get_option('make-update-default-url', RELEASE_INFO_DEFAULT_URL),
      'subdir'              => '',
      'directory_name'      => '',
      'make_directory'      => $make_dir,
      'options'             => array(),
    );
    // If download components are specified, but not the download
    // type, default to git.
    if (isset($project['download']) && !isset($project['download']['type'])) {
      $project['download']['type'] = 'git';
    }
    if (!isset($project['l10n_url']) && ($project['location'] == RELEASE_INFO_DEFAULT_URL)) {
      $project['l10n_url'] = MAKE_DEFAULT_L10N_SERVER;
    }

    // For convenience: define $request to be compatible with release_info
    // engine.
    // TODO: refactor to enforce 'make' to internally work with release_info
    // keys.
    $request = make_prepare_request($project);

    if ($project['location'] != RELEASE_INFO_DEFAULT_URL && !isset($project['type'])) {
      // Set the cache option based on our '--no-cache' option.
      $cache_before = drush_get_option('cache');
      if (!drush_get_option('no-cache', FALSE)) {
        drush_set_option('cache', TRUE);
      }
      $project_type = release_info_check_project($request, 'core');
      // Restore the previous '--cache' option value.
      drush_set_option('cache', $cache_before);
      $project['download_type'] = ($project_type ? 'core' : 'contrib');
    }
    elseif (!empty($project['type'])) {
      $project['download_type'] = ($project['type'] == 'core' ? 'core' : 'contrib');
    }
    else {
      $project['download_type'] = ($project['name'] == 'drupal' ? 'core' : 'contrib');
    }
    $projects[$project['download_type']][$project['name']] = $project;

  }

  foreach ($projects as $type => $type_projects) {
    foreach ($type_projects as $project) {
      if (make_project_needs_release_info($project)) {
        // For convenience: define $request to be compatible with release_info
        // engine.
        // TODO: refactor to enforce 'make' to internally work with release_info
        // keys.
        $request = make_prepare_request($project, $type);
        // Set the cache option based on our '--no-cache' option.
        $cache_before = drush_get_option('cache');
        if (!drush_get_option('no-cache', FALSE)) {
          drush_set_option('cache', TRUE);
        }
        $release = release_info_fetch($request, '');
        if ($release === FALSE) {
          return FALSE;
        }
        // Restore the previous '--cache' option value.
        drush_set_option('cache', $cache_before);
        if (!isset($project['type'])) {
          // Translate release_info key for project_type to drush make.
          $project['type'] = $request['project_type'];
        }
        if (!isset($project['download'])) {
          $project['download'] = array(
            'type' => 'pm',
            'full_version' => $release['version'],
            'download_link' => $release['download_link'],
            'status url' => $request['status url'],
          );
        }
      }
      if (!empty($md5)) {
        $project['download']['md5'] = $md5;
      }
      if ($ignore_checksums) {
        unset($project['download']['md5']);
      }
      $projects[($project['type'] == 'core' ? 'core' : 'contrib')][$project['name']] = $project;
    }
  }

  // We quit before the actual codebase is made.
  return $projects;
}

/**
 * Hacked version of make_libraries().
 *
 * Process all libraries specified in the make file.
 */
function _make_libraries_hacked($contrib_destination, $info, $build_path, $make_dir) {
  if (empty($info['libraries'])) {
    return array();
  }
  $ignore_checksums = drush_get_option('ignore-checksums');
  foreach ($info['libraries'] as $key => $library) {
    if (!is_string($key) || !is_array($library)) {
      // TODO Print a prettier message.
      continue;
    }
    // Merge the known data onto the library info.
    $library += array(
      'name'                => $key,
      'core'                => $info['core'],
      'build_path'          => $build_path,
      'contrib_destination' => $contrib_destination,
      'subdir'              => '',
      'directory_name'      => $key,
      'make_directory'      => $make_dir,
    );
    if ($ignore_checksums) {
      unset($library['download']['md5']);
    }

    $info['libraries'][$key] = $library;
  }

  return $info['libraries'];
}
