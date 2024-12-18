import React, { useEffect, useRef, useState } from "react";
import { HistoryOutlined, HomeOutlined, PlusOutlined, SettingOutlined } from "@ant-design/icons";
import { Grid, Layout, Menu, Tour, TourStepProps } from "antd";
import { Outlet, useLocation, useNavigate } from "react-router-dom";
import FootContent from "./FootContent";
import NotificationList from "@/components/NotificationList";
import { CSSTransition } from "react-transition-group";
import { useSelector } from "react-redux";
import { RootState } from "@/stores/store";

const { Content, Sider } = Layout;
const { useBreakpoint } = Grid;

const App: React.FC = () => {
  const notificationVisible = useSelector((state: RootState) => state.notification.open);
  const headerItems = [
    {
      key: "index",
      label: "首页",
      icon: React.createElement(HomeOutlined)
    },  
    {
      key: "new_commit",
      label: "新建",
      icon: React.createElement(PlusOutlined)
    },
    {
      key: "commit_history",
      label: "历史",
      icon: React.createElement(HistoryOutlined)
    },
    {
      key: "setting",
      label: "设置",
      icon: React.createElement(SettingOutlined)
    }
  ];
  const sideItemsRef = useRef<(HTMLDivElement | null)[]>([null, null, null, null]);
  const steps: TourStepProps[] = [
    {
      title: "欢迎使用",
      description: "这是您的应用首页。",
      placement: "right",
      target: sideItemsRef.current[0]
    },
    {
      title: "创建",
      description: "点击这里可以快速创建新的备份",
      placement: "right",
      target: sideItemsRef.current[1]
    },
    {
      title: "历史",
      description: "在这里查看您所有的备份历史，便于管理和追踪。",
      placement: "right",
      target: sideItemsRef.current[2]
    },
    {
      title: "设置",
      description: "在设置中，您可以自定义应用的各种选项。",
      placement: "right",
      target: sideItemsRef.current[3]
    }
  ];

  const navigate = useNavigate();
  const location = useLocation();
  const [open, setOpen] = useState(false);
  const openTour = () => setOpen(true);
  useEffect(() => {
    setSelectedKeys([location.pathname.slice(1)]);
  }, [location.pathname]);
  const [selectedKeys, setSelectedKeys] = useState<string[]>([location.pathname.slice(1)]);

  const breakpoint = useBreakpoint();
  const gap: {
    offset: [number, number];
    radius: number;
  } = breakpoint.lg ? { offset: [16, 0], radius: 8 } : { offset: [24, 0], radius: 8 };
  return (
    <>
      <Layout hasSider className='select-none flex flex-row '>
        <Sider breakpoint='lg' className='bg-white'>
          <Menu
            onClick={info => navigate(`/${info.key}`)}
            selectedKeys={selectedKeys}
            className='bg-white'
            style={{
              borderInlineEnd: "none"
            }}>
            {headerItems.map((item, index) => (
              <Menu.Item key={item.key}>
                <div className='flex items-center' ref={el => (sideItemsRef.current[index] = el)}>
                  {item.icon}
                  <span className='ml-4'>{item.label}</span>
                </div>
              </Menu.Item>
            ))}
          </Menu>
        </Sider>
        <Layout className='bg-white'>
          <div>
            <Content>
              <Outlet context={{ openTour }} />
            </Content>
            <CSSTransition
              in={notificationVisible}
              timeout={500}
              classNames='fade'
              unmountOnExit={false}>
              <NotificationList />
            </CSSTransition>
          </div>
          <FootContent />
        </Layout>
      </Layout>
      <Tour
        open={open}
        onClose={() => setOpen(false)}
        steps={steps}
        gap={gap}
      />
    </>
  );
};

export default App;
