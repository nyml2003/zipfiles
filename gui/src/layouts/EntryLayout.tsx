import React from 'react';
import {
  BookOutlined,
  HomeOutlined,
} from '@ant-design/icons';
import type { MenuProps } from 'antd';
import { MenuInfo } from 'rc-menu/lib/interface';
import { Breadcrumb, Layout, Menu, theme } from 'antd';
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
    <Layout hasSider className='h-screen'>
      <Sider
        style={{ background: colorBgContainer }}
        className='
          overflow-y-scroll
          hide-scrollbar
          '
        breakpoint='lg'>
              <Menu mode='inline' defaultSelectedKeys={['1']} defaultOpenKeys={['sub1']} items={headerItems} onClick={handleClick} />
      </Sider>
      <Layout className='px-4 pb-4'>
        <Breadcrumb className='my-4'>
          <Breadcrumb.Item>Home</Breadcrumb.Item>
          <Breadcrumb.Item>List</Breadcrumb.Item>
          <Breadcrumb.Item>App</Breadcrumb.Item>
        </Breadcrumb>
        <Content
          style={{
            borderRadius: borderRadiusLG,
          }}
          className='p-4 overflow-y-scroll hide-scrollbar'>
          <Outlet />
        </Content>
      </Layout>
    </Layout>
  );
};

export default App;
