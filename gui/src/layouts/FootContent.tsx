import React from 'react';
import { MessageOutlined, ProfileOutlined } from '@ant-design/icons';
import { Button, Modal } from 'antd';
import BackUpBuffer from '@/components/BackUpBuffer';]0

const FootContent = () => {
  const [modal, contextHolder] = Modal.useModal();
  return (
    <div className=''>
      {contextHolder}
      <Button
        type='text'
        icon={<ProfileOutlined />}
        onClick={() =>
          modal.confirm({
            title: '备份文件',
            content: <BackUpBuffer />,
            onOk() {
              console.log('确认');
            },
            onCancel() {
              console.log('取消');
            },
          })
        }>
        备份
      </Button>
      <MessageOutlined />
    </div>
  );
};

export default FootContent