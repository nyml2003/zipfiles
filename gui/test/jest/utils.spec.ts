import { buildTree, findFile } from "../../src/utils";

test("buildTree1", () => {
  const paths = [
    {
      createTime: 1730533246.0,
      group: "root",
      mode: 33279,
      name: "big_text_test_file",
      owner: "root",
      path: ".",
      size: 3250125,
      type: 1,
      updateTime: 1730471493.0
    },
    {
      createTime: 1730533246.0,
      group: "root",
      mode: 16895,
      name: "multi_dir_1",
      owner: "root",
      path: ".",
      size: 4096,
      type: 2,
      updateTime: 1730471706.0
    },
    {
      createTime: 1730533246.0,
      group: "root",
      mode: 16895,
      name: "multi_dir_2",
      owner: "root",
      path: ".",
      size: 4096,
      type: 2,
      updateTime: 1730471768.0
    },
    {
      createTime: 1730533246.0,
      group: "root",
      mode: 33279,
      name: "small_text_test_file",
      owner: "root",
      path: ".",
      size: 59,
      type: 1,
      updateTime: 1730470846.0
    },
    {
      createTime: 1730533246.0,
      group: "root",
      mode: 33279,
      name: "big_text_test_file",
      owner: "root",
      path: "multi_dir_1",
      size: 3250125,
      type: 1,
      updateTime: 1730471706.0
    },
    {
      createTime: 1730533246.0,
      group: "root",
      mode: 33279,
      name: "small_text_test_file",
      owner: "root",
      path: "multi_dir_1",
      size: 59,
      type: 1,
      updateTime: 1730470873.0
    },
    {
      createTime: 1730533246.0,
      group: "root",
      mode: 33279,
      name: "big_multi_dir_text_test_file",
      owner: "root",
      path: "multi_dir_2",
      size: 3231074,
      type: 1,
      updateTime: 1730471700.0
    },
    {
      createTime: 1730533246.0,
      group: "root",
      mode: 33279,
      name: "small_multi_dir_text_test_file",
      owner: "root",
      path: "multi_dir_2",
      size: 79,
      type: 1,
      updateTime: 1730470913.0
    }
  ];
  const ans = {
    children: [
      {
        createTime: 1730533246,
        group: "root",
        mode: 33279,
        name: "big_text_test_file",
        owner: "root",
        size: 3250125,
        type: 1,
        path: "",
        updateTime: 1730471493
      },
      {
        createTime: 1730533246,
        group: "root",
        mode: 16895,
        name: "multi_dir_1",
        owner: "root",
        size: 4096,
        type: 2,
        path: "",
        updateTime: 1730471706
      },
      {
        createTime: 1730533246,
        group: "root",
        mode: 16895,
        name: "multi_dir_2",
        owner: "root",
        size: 4096,
        type: 2,
        path: "",
        updateTime: 1730471768
      },
      {
        createTime: 1730533246,
        group: "root",
        mode: 33279,
        name: "small_text_test_file",
        owner: "root",
        size: 59,
        type: 1,
        path: "",
        updateTime: 1730470846
      }
    ],
    name: "",
    subDir: [
      {
        name: "multi_dir_1",
        subDir: [],
        children: [
          {
            createTime: 1730533246,
            group: "root",
            mode: 33279,
            name: "big_text_test_file",
            owner: "root",
            size: 3250125,
            type: 1,
            path: "multi_dir_1",
            updateTime: 1730471706
          },
          {
            createTime: 1730533246,
            group: "root",
            mode: 33279,
            name: "small_text_test_file",
            owner: "root",
            size: 59,
            type: 1,
            path: "multi_dir_1",
            updateTime: 1730470873
          }
        ]
      },
      {
        name: "multi_dir_2",
        subDir: [],
        children: [
          {
            createTime: 1730533246,
            group: "root",
            mode: 33279,
            name: "big_multi_dir_text_test_file",
            owner: "root",
            size: 3231074,
            type: 1,
            path: "multi_dir_2",
            updateTime: 1730471700
          },
          {
            createTime: 1730533246,
            group: "root",
            mode: 33279,
            name: "small_multi_dir_text_test_file",
            owner: "root",
            size: 79,
            type: 1,
            path: "multi_dir_2",
            updateTime: 1730470913
          }
        ]
      }
    ]
  };
  const tree = buildTree(paths);
  expect(tree).toEqual(ans);

  const filesInMultiDir1 = findFile(tree, "multi_dir_1");
  expect(filesInMultiDir1).toEqual([
    {
      createTime: 1730533246,
      group: "root",
      mode: 33279,
      name: "big_text_test_file",
      owner: "root",
      size: 3250125,
      type: 1,
      path: "multi_dir_1",
      updateTime: 1730471706
    },
    {
      createTime: 1730533246,
      group: "root",
      mode: 33279,
      name: "small_text_test_file",
      owner: "root",
      size: 59,
      type: 1,
      path: "multi_dir_1",
      updateTime: 1730470873
    }
  ]);
});

// test('buildTree2', () => {
//   const paths = [
//     {
//       path: '.',
//       name: '1',
//     },
//     {
//       path: '.',
//       name: '2',

//     },
//     {
//       path: '.',
//       name: '3',
//     },
//     {
//       path: '3',
//       name: '4',
//     },
//     {
//       path: '3',
//       name: '5',
//     },
//     {
//       path: '3/7/8/9',
//       name: '6',
//     },
//   ];
//   const ans = {
//     children: [
//       {
//         name: '1',
//         path: '',
//       },
//       {
//         name: '2',
//         path: '',
//       },
//       {
//         name: '3',
//         path: '',
//       },
//     ],
//     name: '',
//     subDir: [
//       {
//         name: '3',
//         subDir: [
//           {
//             name: '7',
//             subDir: [
//               {
//                 name: '8',
//                 subDir: [
//                   {
//                     name: '9',
//                     subDir: [],
//                     children: [
//                       {
//                         name: '6',
//                         path: '3/7/8/9',
//                       },
//                     ],
//                   },
//                 ],
//                 children: [],
//               },
//             ],
//             children: [],
//           },
//         ],
//         children: [
//           {
//             name: '4',
//             path: '3',
//           },
//           {
//             name: '5',
//             path: '3',
//           },
//         ],
//       },
//     ],
//   };
//   const tree = buildTree(paths);
//   expect(tree).toEqual(ans);
//   const filesIn3 = findFile(tree., '3');
//   expect(filesIn3).toEqual([
//     {
//       name: '4',
//       path: '3',
//     },
//     {
//       name: '5',
//       path: '3',
//     },
//   ]);
// });

test("buildTree3", () => {
  const paths = [
    {
      name: "vjywpybkws",
      type: 3,
      createTime: 1240813713,
      updateTime: 51915824,
      size: 0,
      owner: "Larry Miller",
      group: "Kenneth Wilson",
      mode: 81,
      path: "lnbw"
    },
    {
      name: "hwjwrz",
      type: 7,
      createTime: 104244021,
      updateTime: 530127957,
      size: 0,
      owner: "Barbara Clark",
      group: "Jennifer Martin",
      mode: 184,
      path: "iiplbr"
    },
    {
      name: "uwtw",
      type: 2,
      createTime: 1322074928,
      updateTime: 725371069,
      size: 0,
      owner: "Carol Young",
      group: "Michael Johnson",
      mode: 616,
      path: "iiplbr"
    },
    {
      name: "sfrbsuw",
      type: 3,
      createTime: 628607819,
      updateTime: 1542338554,
      size: 0,
      owner: "Elizabeth Harris",
      group: "Gary Brown",
      mode: 494,
      path: "lnbw"
    },
    {
      name: "ltonngwgyc",
      type: 4,
      createTime: 832062900,
      updateTime: 1345185557,
      size: 0,
      owner: "William Johnson",
      group: "Deborah Jackson",
      mode: 260,
      path: "iiplbr"
    },
    {
      name: "jsukrnogl",
      type: 1,
      createTime: 562881093,
      updateTime: 924327360,
      size: 46689,
      owner: "Jessica Garcia",
      group: "Melissa Martin",
      mode: 636,
      path: "lnbw"
    },
    {
      name: "kdixupm",
      type: 2,
      createTime: 883037900,
      updateTime: 925333299,
      size: 0,
      owner: "Helen Thompson",
      group: "Kimberly Smith",
      mode: 549,
      path: "."
    },
    {
      name: "nlhqcmle",
      type: 0,
      createTime: 1426235840,
      updateTime: 272001640,
      size: 0,
      owner: "Shirley Harris",
      group: "Steven Thompson",
      mode: 268,
      path: "usr"
    },
    {
      name: "vsomghs",
      type: 0,
      createTime: 694485080,
      updateTime: 1452844396,
      size: 0,
      owner: "Frank Perez",
      group: "Frank Rodriguez",
      mode: 634,
      path: "kdixupm"
    }
  ];
  console.log(JSON.stringify(buildTree(paths), null, 2));
});

test("buildTree4", () => {
  const paths = [
    {
      name: "xduugel",
      type: 3,
      createTime: 996085899,
      updateTime: 1494432823,
      size: 0,
      owner: "Shirley Jackson",
      group: "Linda Davis",
      mode: 602,
      path: "oihtn"
    },
    {
      name: "ajcjojd",
      type: 2,
      createTime: 1700425050,
      updateTime: 1352112752,
      size: 0,
      owner: "Lisa Walker",
      group: "Shirley Martin",
      mode: 307,
      path: ""
    },
    {
      name: "kkbam",
      type: 3,
      createTime: 1706580177,
      updateTime: 756038927,
      size: 0,
      owner: "Charles Davis",
      group: "Susan Rodriguez",
      mode: 327,
      path: ""
    },
    {
      name: "lprcc",
      type: 4,
      createTime: 978977686,
      updateTime: 1249657932,
      size: 0,
      owner: "Karen Garcia",
      group: "Melissa Johnson",
      mode: 21,
      path: ""
    },
    {
      name: "ckreqh",
      type: 2,
      createTime: 457205228,
      updateTime: 525368124,
      size: 0,
      owner: "William Perez",
      group: "Mary Davis",
      mode: 184,
      path: "usr"
    },
    {
      name: "lxdaxoc",
      type: 8,
      createTime: 78738764,
      updateTime: 988931386,
      size: 0,
      owner: "Sharon Garcia",
      group: "Joseph Hall",
      mode: 257,
      path: "usr"
    },
    {
      name: "tcibr",
      type: 3,
      createTime: 477332496,
      updateTime: 1365305913,
      size: 0,
      owner: "Karen Gonzalez",
      group: "Scott Lewis",
      mode: 6,
      path: ""
    },
    {
      name: "wvra",
      type: 2,
      createTime: 1520059085,
      updateTime: 912756718,
      size: 0,
      owner: "Jessica Lewis",
      group: "Betty Clark",
      mode: 496,
      path: "ajcjojd"
    },
    {
      name: "onuyxxr",
      type: 3,
      createTime: 1387125941,
      updateTime: 325643213,
      size: 0,
      owner: "Mark Martinez",
      group: "Anna Jones",
      mode: 3,
      path: "ajcjojd"
    }
  ];
  console.log(JSON.stringify(buildTree(paths), null, 2));
});
