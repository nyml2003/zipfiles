import React from 'react';
import { HomeOutlined, SettingOutlined, UploadOutlined } from '@ant-design/icons';
import type { MenuProps } from 'antd';
import { MenuInfo } from 'rc-menu/lib/interface';
import { Layout, Menu, theme } from 'antd';
import { Outlet, useNavigate } from 'react-router-dom';
import FootContent from './FootContent';

const { Content, Sider } = Layout;

const headerItems: MenuProps['items'] = [
  {
    key: 'index',
    label: '首页',
    icon: React.createElement(HomeOutlined),
  },
  {
    key: 'new_commit',
    label: '新建提交',
    icon: React.createElement(UploadOutlined),
  },
  {
    key: 'commit_history',
    label: '提交历史',
    icon: React.createElement(UploadOutlined),
  },
  {
    key: 'setting',
    label: '设置',
    icon: React.createElement(UploadOutlined),
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
      <Sider breakpoint='lg' className='overflow-hidden bg-white'>
        <Menu
          //mode='inline'
          defaultSelectedKeys={['1']}
          defaultOpenKeys={['sub1']}
          items={headerItems}
          onClick={handleClick}
          className='bg-white'
          style={{
            borderInlineEnd: 'none',
          }}
        />
      </Sider>
      <Layout className='h-screen flex-1 w-full'>
        <Content className='flex flex-1'>
          <Outlet />
        </Content>
        <FootContent />
      </Layout>
    </Layout>
  );
};

export default App;
