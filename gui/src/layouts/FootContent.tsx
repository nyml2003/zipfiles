import Button from "@/components/Button";
import { toggleNotification } from "@/stores/NotificationReducer";
import { MessageOutlined } from "@ant-design/icons";
import React from "react";
import { useDispatch } from "react-redux";

const FootContent = () => {
  const dispatch = useDispatch();
  return (
    <div className='bg-white flex flex-row-reverse p-2'>
      <Button onClick={() => dispatch(toggleNotification())} variant='primary'>
        <MessageOutlined />
      </Button>
    </div>
  );
};

export default FootContent;
