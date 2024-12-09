import React, { useEffect, useState } from "react";
import { HistoryOutlined, HomeOutlined, PlusOutlined, SettingOutlined } from "@ant-design/icons";
import { Layout, Menu } from "antd";
import { Outlet, useLocation, useNavigate } from "react-router-dom";
import FootContent from "./FootContent";
import NotificationList from "@/components/NotificationList";
import { CSSTransition } from "react-transition-group";
import { useSelector } from "react-redux";
import { RootState } from "@/stores/store";

const { Content, Sider } = Layout;

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
  // const headerItemsRef = useRef<HTMLElement[]>([]);
  // const steps: TourStepProps[] = [
  //   {
  //     title: "欢迎使用",
  //     description: "这是您的应用首页。",
  //     placement: "right",
  //     target: headerItemsRef.current[0]!
  //   },
  //   {
  //     title: "新建提交",
  //     description: "点击这里可以快速创建新的提交，方便您的工作流程。",
  //     placement: "right",
  //     target: headerItemsRef.current[1]!
  //   },
  //   {
  //     title: "提交历史",
  //     description: "在这里查看您所有的提交历史，便于管理和追踪。",
  //     placement: "right",
  //     target: headerItemsRef.current[2]!
  //   },
  //   {
  //     title: "设置",
  //     description: "在设置中，您可以自定义应用的各种选项。",
  //     placement: "right",
  //     target: headerItemsRef.current[3]!
  //   }
  // ];

  const navigate = useNavigate();
  const location = useLocation();

  useEffect(() => {
    setSelectedKeys([location.pathname.slice(1)]);
  }, [location.pathname]);
  const [selectedKeys, setSelectedKeys] = useState<string[]>([location.pathname.slice(1)]);
  return (
    <>
      <Layout hasSider className='select-none flex flex-row h-screen'>
        <Sider breakpoint='lg' className='bg-white'>
          <Menu
            onClick={info => navigate(`/${info.key}`)}
            selectedKeys={selectedKeys}
            items={headerItems}
            className='bg-white'
            style={{
              borderInlineEnd: "none"
            }}></Menu>
        </Sider>
        <Layout className="bg-white">
          <div>
            <Content>
              <Outlet />
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
    </>
  );
};

export default App;
