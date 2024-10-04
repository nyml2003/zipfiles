import React from 'react';
import { BookOutlined, HomeOutlined, SettingOutlined } from '@ant-design/icons';
import type { MenuProps } from 'antd';
import { MenuInfo } from 'rc-menu/lib/interface';
import { Layout, Menu, theme } from 'antd';
import { Outlet, useNavigate } from 'react-router-dom';

const { Content, Sider } = Layout;

const headerItems: MenuProps['items'] = [
  {
    key: 'index',
    label: '首页',
    icon: React.createElement(HomeOutlined),
  },
  {
    key: 'explorer',
    label: '文件列表',
    icon: React.createElement(BookOutlined),
  },
  {
    key: 'commit_history',
    label: '提交历史',
    icon: React.createElement(SettingOutlined),
  },
  {
    key: 'setting',
    label: '设置',
    icon: React.createElement(SettingOutlined),
  },
  {
    key: 'test',
    label: '测试',
    icon: React.createElement(SettingOutlined),
  },
];

const App: React.FC = () => {
  const navigate = useNavigate();
  const {
    token: { colorBgContainer, borderRadiusLG },
  } = theme.useToken();

  const handleClick = (info: MenuInfo) => {
    if (info.key === 'github') {
      navigate('/index');
      return;
    }
    navigate(info.key);
  };

  return (
    <Layout hasSider className='h-screen select-none'>
      <Sider
        style={{ background: colorBgContainer }}
        breakpoint='lg'
        className='overflow-hidden lg:rounded-r-2xl rounded-r-xl'>
        <Menu
          mode='inline'
          defaultSelectedKeys={['1']}
          defaultOpenKeys={['sub1']}
          items={headerItems}
          onClick={handleClick}
        />
      </Sider>
      <Layout className='h-screen flex-1 w-full'>
        <Content
          style={{
            borderRadius: borderRadiusLG,
          }}
          className='flex flex-1'>
          <Outlet />
        </Content>
      </Layout>
    </Layout>
  );
};

export default App;
