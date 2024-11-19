import { toggleNotification } from "@/stores/NotificationReducer";
import { MessageOutlined } from "@ant-design/icons";
import { Button } from "antd";
import React from "react";
import { useDispatch } from "react-redux";

const FootContent = () => {
  const dispatch = useDispatch();
  return (
    <div className='bg-white flex flex-row-reverse p-2'>
      <Button icon={<MessageOutlined />} onClick={() => dispatch(toggleNotification())} type='text'>
      </Button>
    </div>
  );
};

export default FootContent;
