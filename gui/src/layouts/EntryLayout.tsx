import React from 'react';
import { Button, Layout, Nav, Space, Tooltip } from '@douyinfe/semi-ui';
import { Link, Outlet } from 'react-router-dom';
import { IconHome, IconArticle, IconGithubLogo } from '@douyinfe/semi-icons';
import { useDarkMode } from '@/hooks/useDarkMode';
function openGithub() {
  window.open('https://github.com');
}
const EntryLayout = () => {
  const { Header, Footer, Content } = Layout;
  const { toggleMode, IconDarkMode, mode } = useDarkMode();
  return (
    <Layout className='flex flex-col h-screen'>
      <Header className='bg-white shadow-md fixed h-16 w-full z-10'>
        <Nav
          mode='horizontal'
          defaultOpenKeys={['home']}
          className='h-full overflow-x-auto hide-scrollbar'>
          <Nav.Header>
            <Space>
              <Link relative='path' to='/'>
                <Nav.Item icon={<IconHome />} text='Home' itemKey={'home'} />
              </Link>
              <Link to='/explorer'>
                <Nav.Item icon={<IconArticle />} text='Explorer' itemKey={'explorer'} />
              </Link>
              <Link to='/test'>
                <Nav.Item icon={<IconArticle />} text='Test' itemKey={'test'} />
              </Link>
            </Space>
          </Nav.Header>
          <Nav.Footer>
            <Space>
              {/* <Input placeholder="Search" prefix={<IconSearch />} showClear /> */}
              <Tooltip content={`Swith to ${mode} mode`}>
                <Button
                  icon={
                    <IconDarkMode
                      style={{
                        color: 'var(--semi-color-text-2)',
                      }}
                      size='large'
                    />
                  }
                  theme='borderless'
                  onClick={toggleMode}
                  type='primary'
                />
              </Tooltip>
              <Tooltip content='Github'>
                <Button
                  icon={
                    <IconGithubLogo
                      style={{
                        color: 'var(--semi-color-text-2)',
                      }}
                      size='large'
                    />
                  }
                  theme='borderless'
                  onClick={openGithub}
                  type='primary'
                  size='large'
                />
              </Tooltip>
            </Space>
          </Nav.Footer>
        </Nav>
      </Header>
      <Content className='mt-16 flex-1 overflow-hidden'>
        <Outlet />
      </Content>
    </Layout>
  );
};

export default EntryLayout;
