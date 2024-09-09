/*
 * @Author: shawnxiao 597035529@qq.com
 * @Date: 2022-11-26 21:00:08
 * @LastEditors: shawnxiao 597035529@qq.com
 * @LastEditTime: 2022-11-26 21:00:12
 * @FilePath: \react\webpack5-ts-react18\mock\controller\user.js
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
module.exports = [
  {
    url: '/api/sso/login/checkStatus',
    type: 'get',
    response: {
      code: 200,
      message: 'success',
      data: {
        login: true,
        binding: true,
        tokenId: 'tokenId380530530'
      }
    }
  },
  {
    url: '/api/user/info',
    type: 'get',
    response: {
      code: 200,
      message: 'success',
      data: {
        name: 'shawnxiao',
        avatar: '',
        account: 'account',
        mobile: 'mobile',
        role: 'role',
        id: 'id'
      }
    }
  }
]
