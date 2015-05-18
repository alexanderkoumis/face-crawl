{
  'targets': [
    {
      'target_name': 'face_crawl',
      'include_dirs': [ '<!@(pkg-config --cflags opencv)' ],
      'sources': [
        'face_crawl.cpp',
        ],
      'cflags':[ '-std=gnu++11' ],
      'libraries': [
            '<!@(pkg-config --libs opencv)'
        ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'OTHER_CFLAGS': [
              '-std=c++11', '-stdlib=libc++', '-Wc++11-extensions', '-mmacosx-version-min=10.8'
            ],
          },
          'defines': [
          ]
        }],
        ['OS=="linux"', {
          'defines': [
          ]
        }],
        ['OS=="solaris"', {
          'defines': [
          ]
        }]
      ]
    }
  ]
}
