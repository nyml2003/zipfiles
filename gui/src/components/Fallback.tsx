import { Spin } from '@douyinfe/semi-ui'
import React from 'react'

const Fallback: React.FC = () => (
  <div className='flex justify-center items-center h-full'>
    <Spin size='large' />
  </div>
)

export default Fallback
