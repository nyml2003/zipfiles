import { buildTree, findFile } from '../../src/utils/index';

test('buildTree1', () => {
  const paths = [
    {
      createTime: 1730533246.0,
      group: 'root',
      mode: 33279,
      name: 'big_text_test_file',
      owner: 'root',
      path: '.',
      size: 3250125,
      type: 1,
      updateTime: 1730471493.0,
    },
    {
      createTime: 1730533246.0,
      group: 'root',
      mode: 16895,
      name: 'multi_dir_1',
      owner: 'root',
      path: '.',
      size: 4096,
      type: 2,
      updateTime: 1730471706.0,
    },
    {
      createTime: 1730533246.0,
      group: 'root',
      mode: 16895,
      name: 'multi_dir_2',
      owner: 'root',
      path: '.',
      size: 4096,
      type: 2,
      updateTime: 1730471768.0,
    },
    {
      createTime: 1730533246.0,
      group: 'root',
      mode: 33279,
      name: 'small_text_test_file',
      owner: 'root',
      path: '.',
      size: 59,
      type: 1,
      updateTime: 1730470846.0,
    },
    {
      createTime: 1730533246.0,
      group: 'root',
      mode: 33279,
      name: 'big_text_test_file',
      owner: 'root',
      path: 'multi_dir_1',
      size: 3250125,
      type: 1,
      updateTime: 1730471706.0,
    },
    {
      createTime: 1730533246.0,
      group: 'root',
      mode: 33279,
      name: 'small_text_test_file',
      owner: 'root',
      path: 'multi_dir_1',
      size: 59,
      type: 1,
      updateTime: 1730470873.0,
    },
    {
      createTime: 1730533246.0,
      group: 'root',
      mode: 33279,
      name: 'big_multi_dir_text_test_file',
      owner: 'root',
      path: 'multi_dir_2',
      size: 3231074,
      type: 1,
      updateTime: 1730471700.0,
    },
    {
      createTime: 1730533246.0,
      group: 'root',
      mode: 33279,
      name: 'small_multi_dir_text_test_file',
      owner: 'root',
      path: 'multi_dir_2',
      size: 79,
      type: 1,
      updateTime: 1730470913.0,
    },
  ];
  const ans = {
    children: [
      {
        createTime: 1730533246,
        group: 'root',
        mode: 33279,
        name: 'big_text_test_file',
        owner: 'root',
        size: 3250125,
        type: 1,
        path: '',
        updateTime: 1730471493,
      },
      {
        createTime: 1730533246,
        group: 'root',
        mode: 16895,
        name: 'multi_dir_1',
        owner: 'root',
        size: 4096,
        type: 2,
        path: '',
        updateTime: 1730471706,
      },
      {
        createTime: 1730533246,
        group: 'root',
        mode: 16895,
        name: 'multi_dir_2',
        owner: 'root',
        size: 4096,
        type: 2,
        path: '',
        updateTime: 1730471768,
      },
      {
        createTime: 1730533246,
        group: 'root',
        mode: 33279,
        name: 'small_text_test_file',
        owner: 'root',
        size: 59,
        type: 1,
        path: '',
        updateTime: 1730470846,
      },
    ],
    name: '',
    subDir: [
      {
        name: 'multi_dir_1',
        subDir: [],
        children: [
          {
            createTime: 1730533246,
            group: 'root',
            mode: 33279,
            name: 'big_text_test_file',
            owner: 'root',
            size: 3250125,
            type: 1,
            path: 'multi_dir_1',
            updateTime: 1730471706,
          },
          {
            createTime: 1730533246,
            group: 'root',
            mode: 33279,
            name: 'small_text_test_file',
            owner: 'root',
            size: 59,
            type: 1,
            path: 'multi_dir_1',
            updateTime: 1730470873,
          },
        ],
      },
      {
        name: 'multi_dir_2',
        subDir: [],
        children: [
          {
            createTime: 1730533246,
            group: 'root',
            mode: 33279,
            name: 'big_multi_dir_text_test_file',
            owner: 'root',
            size: 3231074,
            type: 1,
            path: 'multi_dir_2',
            updateTime: 1730471700,
          },
          {
            createTime: 1730533246,
            group: 'root',
            mode: 33279,
            name: 'small_multi_dir_text_test_file',
            owner: 'root',
            size: 79,
            type: 1,
            path: 'multi_dir_2',
            updateTime: 1730470913,
          },
        ],
      },
    ],
  };
  const tree = buildTree(paths);
  expect(tree).toEqual(ans);

  const filesInMultiDir1 = findFile(tree, 'multi_dir_1');
  expect(filesInMultiDir1).toEqual([
    {
      createTime: 1730533246,
      group: 'root',
      mode: 33279,
      name: 'big_text_test_file',
      owner: 'root',
      size: 3250125,
      type: 1,
      path: 'multi_dir_1',
      updateTime: 1730471706,
    },
    {
      createTime: 1730533246,
      group: 'root',
      mode: 33279,
      name: 'small_text_test_file',
      owner: 'root',
      size: 59,
      type: 1,
      path: 'multi_dir_1',
      updateTime: 1730470873,
    },
  ]);
});

test('buildTree2', () => {
  const paths = [
    {
      path: '.',
      name: '1',
    },
    {
      path: '.',
      name: '2',
    },
    {
      path: '.',
      name: '3',
    },
    {
      path: '3',
      name: '4',
    },
    {
      path: '3',
      name: '5',
    },
    {
      path: '3/7/8/9',
      name: '6',
    },
  ];
  const ans = {
    children: [
      {
        name: '1',
        path: '',
      },
      {
        name: '2',
        path: '',
      },
      {
        name: '3',
        path: '',
      },
    ],
    name: '',
    subDir: [
      {
        name: '3',
        subDir: [
          {
            name: '7',
            subDir: [
              {
                name: '8',
                subDir: [
                  {
                    name: '9',
                    subDir: [],
                    children: [
                      {
                        name: '6',
                        path: '3/7/8/9',
                      },
                    ],
                  },
                ],
                children: [],
              },
            ],
            children: [],
          },
        ],
        children: [
          {
            name: '4',
            path: '3',
          },
          {
            name: '5',
            path: '3',
          },
        ],
      },
    ],
  };
  const tree = buildTree(paths);
  expect(tree).toEqual(ans);
  const filesIn3 = findFile(tree, '3');
  expect(filesIn3).toEqual([
    {
            name: '4',
            path: '3',
          },
          {
            name: '5',
            path: '3',
    },
  ]);
});

// test('buildTree3', () => {
//   const paths = [
//     [
//                         {
//                                 "createTime" : 1730533246.0,
//                                 "group" : "root",
//                                 "mode" : 33279,
//                                 "name" : "small_picture.jpg",
//                                 "owner" : "root",
//                                 "path" : "picture",
//                                 "size" : 550070,
//                                 "type" : 1,
//                                 "updateTime" : 1675767711.0
//                         },
//                         {
//                                 "createTime" : 1730533246.0,
//                                 "group" : "root",
//                                 "mode" : 33279,
//                                 "name" : "small_picture.png",
//                                 "owner" : "root",
//                                 "path" : "picture",
//                                 "size" : 302514,
//                                 "type" : 1,
//                                 "updateTime" : 1688438810.0
//                         },
//                         {
//                                 "createTime" : 1730533246.0,
//                                 "group" : "root",
//                                 "mode" : 33279,
//                                 "name" : "small_video.mp4",
//                                 "owner" : "root",
//                                 "path" : "video",
//                                 "size" : 97844606,
//                                 "type" : 1,
//                                 "updateTime" : 1722764481.0
//                         }
//     ]
//   ]